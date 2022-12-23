CC=gcc
CFLAGS=-pedantic -Wall -g -lncurses
BIN=sn88k

all: $(BIN)

$(BIN): *.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(BIN)
