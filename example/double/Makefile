CC = gcc
CFLAG = -Wall -std=c99

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
EXEC = double

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAG) -o $@ $^

$(OBJ): $(SRC)
	$(CC) $(CFLAG) -c $^

.PHONY: cleanall cleanobj cleanexec

cleanall: cleanobj cleanexec

cleanobj:
	-rm -rf $(OBJ)

cleanexec:
	-rm -rf $(EXEC)