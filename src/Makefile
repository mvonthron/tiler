#
# Tiler
#

CC = gcc
CFLAGS = -Wall
LFLAGS = -lX11 -lm -lXinerama
DEBUG = -g #-pg
BIN = tiler
OBJS = geometries.o keybindings.o config.o callbacks.o xactions.o utils.o tiler.o

# installation
BINDIR = /usr/bin
MANPAGE = tiler.1
MANDIR = /usr/share/man/man1
CONFDIST = tiler.conf.dist
CONF = $(HOME)/.config/tiler.conf

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(DEBUG) $(LFLAGS) -o $(BIN) $?

.c.o:
	$(CC) $(DEBUG) $(CFLAGS) -o $@ -c $?

clean:
	rm -f *.o

mrproper: clean
	rm -f $(BIN)

install: 
	@if [ `whoami` = "root" ] ; then 								\
		echo "install -m 755 $(BIN) $(BINDIR)";				\
		install -m 755 $(BIN) $(BINDIR);							\
		echo "install -m 644 $(MANPAGE) $(MANDIR)";		\
		install -m 644 $(MANPAGE) $(MANDIR);					\
		echo "install -m 644 $(CONFDIST) $(CONF)";		\
		install -m 644 $(CONFDIST) $(CONF);						\
	else																						\
		echo "Need to go root?";											\
	fi

uninstall:
	@if [ `whoami` = "root" ] ; then 								\
		echo "rm -f $(BINDIR)/$(BIN)";								\
		rm -f $(BINDIR)/$(BIN);												\
		echo "rm -f $(MANDIR)/$(MANPAGE)";						\
		rm -f $(MANDIR)/$(MANPAGE);										\
		echo "rm -f $(CONF)";													\
		rm -f $(CONF);																\
	else																						\
		echo "Need to go root?";											\
	fi
