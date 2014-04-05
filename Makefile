CC=gcc
FLAGS=$(CFLAGS) -std=c99 -O3
SOURCES=shoco.c test_input.c
HEADERS=shoco.h
EXECUTABLE=test_input

all: $(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)

