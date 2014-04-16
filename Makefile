FLAGS=$(CFLAGS) -std=c99 -O3 -Wall
SOURCES=shoco.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=shoco.h shoco_table.h
GENERATOR=generate_successor_table.py
TRAINING_DATA_DIR=training_data
TRAINING_DATA=$(wildcard training_data/*.txt)
TABLES_DIR=tables
TABLES=$(TABLES_DIR)/text_en.h $(TABLES_DIR)/words_en.h $(TABLES_DIR)/filepaths.h

.PHONY: all
all: shoco

shoco: shoco-bin.o $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@

test_input: test_input.o $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@

$(OBJECTS): %.o: %.c $(HEADERS)
	$(CC) $(FLAGS) $< -c

shoco_table.h: $(TABLES_DIR)/words_en.h
	cp $< $@

.PHONY: tables
tables: $(TABLES)

$(TABLES_DIR)/text_en.h: $(TRAINING_DATA) $(GENERATOR)
	python $(GENERATOR) $(TRAINING_DATA) -o $@

$(TABLES_DIR)/words_en.h: $(TRAINING_DATA) $(GENERATOR)
	python $(GENERATOR) --split=whitespace --strip=punctuation $(TRAINING_DATA) -o $@

$(TABLES_DIR)/dictionary.h: /usr/share/dict/words $(GENERATOR)
	python $(GENERATOR) $< -o $@

# Warning: This is *slow*! Use pypy when possible
$(TABLES_DIR)/filepaths.h: $(GENERATOR)
	find / -print 2>/dev/null | python $(GENERATOR) --optimize-encoding -o $@

.PHONY: check
check: tests

tests: tests.o $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) $< -o $@
	./tests

.PHONY: clean
clean:
	rm *.o
