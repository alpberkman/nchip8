OUT=chip8

CC=gcc
CFLAGS=-O2
LIBS=-lncurses

SRC = main.c chip8.c term_fb.c



make: $(SRC)
	$(CC) $(CFLAGS) $(LIBS) -o $(OUT) $(SRC)

clean:
	rm $(OUT)
