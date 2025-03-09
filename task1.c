#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 256

void initiate_download(const char *filename, const char *url, const char *timeout) {
    char command[MAX_LINE];
    if (timeout) {
        // add the execlp() command here directly
        snprintf(command, sizeof(command), "curl -m %s -o %s -s %s", timeout, filename, url);
    } else {
        snprintf(command, sizeof(command), "curl -o %s -s %s", filename, url);
    }
    execlp("sh", "sh", "-c", command, NULL);
    perror("execlp failed");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file> <max_processes>\n", argv[0]);
        return 1;
    }

    int max_processes = atoi(argv[2]); // unknown function used here
                                        // he said use strol instead of atoi
    if (max_processes <= 0) {
        printf("Invalid max_processes value. Must be a positive integer.\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE];
    int line_number = 0;
    int active_processes = 0;

    while (fgets(line, sizeof(line), file)) 
    {
        line_number++;

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') 
        {
            line[len - 1] = '\0';
        }

        // Parse line
        /*
        WARNING: DON'T USE sscanf() FUNCTION TO PARSE THE LINE.
        INSTEAD, USE strtok() FUNCTION TO PARSE THE LINE.
        */
        char output_filename = strtok(line, " ");
        char url = strtok(NULL, " ");
        char timeout = strtok(NULL, " ");
       
        /*
        char output_filename[MAX_LINE], url[MAX_LINE], timeout[MAX_LINE];
        char *timeout_arg = NULL;
        int num_fields = sscanf(line, "%s %s %s", output_filename, url, timeout); // unknown function used here
        // he specified to use strtok instead of sscanf
        */

        if (num_fields < 2) 
        {
            printf("Skip bad format line #%d\n", line_number);
            continue;
        }
        if (num_fields == 3) 
        {
            timeout_arg = timeout;
        }

        // Wait for available process slots
        if (active_processes >= max_processes) {
            pid_t completed_pid = wait(NULL);
            active_processes--;
            printf("Process %d completed.\n", completed_pid);
        }

        // Fork a new process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            fclose(file);
            return 1;
        }

        if (pid == 0) {  // Child process
            printf("Process %d processing line #%d\n", getpid(), line_number);
            initiate_download(output_filename, url, timeout_arg);
        } else {  // Parent process
            active_processes++;
        }
    }
    fclose(file);

    // Wait for remaining processes
    while (active_processes > 0) {
        pid_t completed_pid = wait(NULL);
        printf("Process %d completed.\n", completed_pid);
        active_processes--;
    }
    return 0;
}
