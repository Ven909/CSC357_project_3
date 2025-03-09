#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 256

// Function to initiate download using curl command
void initiate_download(const char *filename, const char *url, const char *timeout) 
{
    if (timeout) {
        execlp("curl", "curl", "-m", timeout, "-o", filename, "-s", url, NULL); // Execute curl command with timeout
    } else {
        execlp("curl", "curl", "-o", filename, "-s", url, NULL);    // Execute curl command without timeout
    }
    perror("execlp failed");    // Print error if execlp fails
    exit(1);
}

int main(int argc, char *argv[]) 
{
    // Check if arguments are valid
    if (argc != 3) {
        printf("Usage: %s <file> <max_processes>\n", argv[0]);
        return 1;
    }

    char *endptr;
    long max_processes = strtol(argv[2], &endptr, 10); // Convert string to long

    // Check if file is valid
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE];    // Line buffer
    int line_number = 0;    // Line number
    int active_processes = 0;   // Active processes count

    // Read file line by line
    while (fgets(line, sizeof(line), file)) 
    {
        line_number++;  // Increment line number

        // Remove newline character from line
        size_t len = strlen(line);
        // Check if line is not empty
        if (len > 0 && line[len - 1] == '\n') 
        {
            line[len - 1] = '\0';   // Replace newline with null terminator
        }

        // Tokenize line using strtok()
        char *output_filename = strtok(line, " "); 
        char *url = strtok(NULL, " ");  
        char *timeout = strtok(NULL, " ");  // Optional timeout

        // Check if output_filename and url are valid
        if (!output_filename || !url) {
            printf("Skipping bad format line #%d\n", line_number);
            continue;
        }

        // Wait for available process slots
        if (active_processes >= max_processes) {
            pid_t completed_pid = wait(NULL);   // Wait for any child process to complete
            active_processes--;                 // Decrement active process count
            printf("Process %d completed.\n", completed_pid);
        }

        // Fork a new process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");  // Print error if fork fails
            fclose(file);       // Close file
            return 1;
        }

        if (pid == 0) {  // Child process
            printf("Process %d processing line #%d\n", getpid(), line_number);  
            initiate_download(output_filename, url, timeout);   // Execute download command
        } else {  // Parent process
            active_processes++; // Increment active process count
        }
    }
    fclose(file);   // Close file

    // Wait for remaining processes
    while (active_processes > 0) {  
        pid_t completed_pid = wait(NULL);   // Wait for any child process to complete
        printf("Process %d completed.\n", completed_pid);   
        active_processes--;                // Decrement active process count
    }
    return 0;
}
