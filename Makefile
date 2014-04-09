FLAGS=$(CFLAGS) -std=c99 -O3 -Wall -funroll-all-loops
SOURCES=shoco.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=shoco.h shoco_table.h
GENERATOR=generate_successor_table.py
TRAINING_DATA_DIR=training_data
TRAINING_DATA=$(wildcard training_data/*.txt)
TABLES_DIR=tables
TABLES=$(TABLES_DIR)/text_en.h $(TABLES_DIR)/words_en.h

.PHONY: all
all: shoco

shoco: shoco-bin.o $(OBJECTS) $(TABLES)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@

test_input: test_input.o $(OBJECTS) $(TABLES)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@

$(OBJECTS): %.o: %.c $(HEADERS)
	$(CC) $(FLAGS) $< -c

.PHONY: tables
tables: $(TABLES)
	rm -f shoco_table.h
	ln -s $(TABLES_DIR)/words_en.h shoco_table.h

$(TABLES_DIR)/text_en.h: $(TRAINING_DATA) $(GENERATOR)
	python $(GENERATOR) $(TRAINING_DATA) -o $@

$(TABLES_DIR)/words_en.h: $(TRAINING_DATA) $(GENERATOR)
	python $(GENERATOR) --split=whitespace --strip=punctuation $(TRAINING_DATA) -o $@
