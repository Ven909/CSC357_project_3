CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
MAIN = task1
SRC = new_task.c

all : $(MAIN)

$(MAIN) : $(SRC)
	$(CC) $(CFLAGS) -g -o $(MAIN) $(SRC)

clean :
	rm -f $(MAIN)
