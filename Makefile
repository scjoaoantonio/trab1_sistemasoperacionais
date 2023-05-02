CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

OBJ = main.o

all: shell

shell: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o shell

main.o: main.c $(HDR)
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(OBJ) shell