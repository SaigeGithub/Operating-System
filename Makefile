CC = gcc
CFLAGS = -Wall
OUTPUT = diskinfo disklist diskget

.PHONY: all $(OUTPUT)
all: $(OUTPUT)

$(OUTPUT): 
	$(CC) $@.c -o $@



