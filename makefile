CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
MAIN = task1
SRC = task1.c

all : $(MAIN)

$(MAIN) : $(SRC)
	$(CC) $(CFLAGS) -o $(MAIN) $(SRC)

clean :
	rm *.o $(MAIN) core
