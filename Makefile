FLAGS=$(CFLAGS) -std=c99 -O3 -Wall -funroll-all-loops
SOURCES=shoco.c test_input.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=shoco.h shoco_table.h
EXE=test_input
GENERATOR=generate_successor_table.py
TRAINING_DATA_DIR=training_data
TRAINING_DATA=$(TRAINING_DATA_DIR)/%.txt
TABLES_DIR=tables
TABLES=$(TABLES_DIR)/dictionary.h $(TABLES_DIR)/text_en.h $(TABLES_DIR)/words_en.h

.PHONY: all
all: $(EXE)

$(EXE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): %.o: %.c $(HEADERS)
		$(CC) $(FLAGS) $< -c

.PHONY: tables
tables: $(TABLES)

$(TABLES_DIR)/dictionary.h: $(TRAINING_DATA_DIR)/dictionary.txt $(GENERATOR)
	pypy $(GENERATOR) $< -o $@

$(TABLES_DIR)/text_en.h: $(TRAINING_DATA_DIR)/sherlock_holmes.txt $(GENERATOR)
	pypy $(GENERATOR) $< -o $@

$(TABLES_DIR)/words_en.h: $(TRAINING_DATA_DIR)/sherlock_holmes.txt $(GENERATOR)
	pypy $(GENERATOR) --split=whitespace --strip=punctuation $< -o $@
