CC = gcc
CFLAG = -Wall -std=c99

SRC = $(wildcard *.c)

EXEC = ../../bin/double

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAG) -o $@ $^

.PHONY: cleanall cleanobj cleanexec

cleanall: cleanobj cleanexec

cleanobj:
	-rm -rf $(OBJ)

cleanexec:
	-rm -rf $(EXEC)