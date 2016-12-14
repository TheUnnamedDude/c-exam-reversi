CC=gcc
CFLAGS=-I -Wall -Wextra -std=c11 -O0 -g -Dncurses
HEADERS=board.h gamelogic.h filelog.h
EXEC=exam
SOURCES=ncurses_board.c main.c gamelogic.c filelog.c
OBJECTS=$(SOURCES:.c=.o)
LDFLAGS=-lncurses

all: exam

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJECTS) $(CFLAGS)

.o: $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)
    
clean:
	rm -f exam *.o
