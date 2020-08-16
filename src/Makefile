# obj

.PHONY: clean all

ROOT := ..
INCLUDE_PATH := $(ROOT)/include

CC := gcc
C_FLAG := -Wall -std=c99
C_INC := -I $(INCLUDE_PATH)

AR := ar
AR_FLAG := rcsv

RM := rm
RM_FLAG := -rf

TARGET := libcii.a
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(AR_FLAG) $@ $^

%.o: %.c
	$(CC) $(C_FLAG) $(C_INC) -g -c $^ -o $@

clean:
	$(RM) $(RM_FLAG) $(OBJ) $(TARGET)