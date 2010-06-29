#
# Tiler
# Copyright (c) 2010 Manuel Vonthron <manuel.vonthron@acadis.org>
#

CC = gcc
CFLAGS = -Wall -g
LFLAGS = -lX11 -lm
DEBUG = #-g
BIN = tiler
OBJS = keybindings.o config.o callbacks.o xactions.o utils.o tiler.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(DEBUG) $(LFLAGS) -o $(BIN) $?

.c.o:
	$(CC) $(DEBUG) $(CFLAGS) -o $@ -c $?

clean:
	rm -f *.o

mrproper: clean
	rm -f $(BIN)
