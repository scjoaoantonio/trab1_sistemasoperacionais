CC = gcc
CFLAGS = -Wall

OBJ = shellso.o

all: shell

shell: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o shell

shellso.o: shellso.c $(HDR)
	$(CC) $(CFLAGS) -c shellso.c -o shellso.o

clean:
	rm -f $(OBJ) shell