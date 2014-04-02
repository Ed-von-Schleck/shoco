CC=gcc
FLAGS=$(CFLAGS) -std=c11 -g
SOURCES=smateco.c test.c
HEADERS=smateco.h
EXECUTABLE=test

all: $(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)

