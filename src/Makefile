# obj

.PHONY: clean all

ROOT := ..
INCLUDE_PATH := $(ROOT)/include
TARGET_PATH := $(ROOT)/lib

CC := gcc
C_INC := -I $(INCLUDE_PATH)
C_DEBUG := -g
C_FLAG := -Wall -std=c99


AR := ar
AR_FLAG := rcsv

RM := rm
RM_FLAG := -rf

TARGET := $(TARGET_PATH)/libcii.a
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, $(ROOT)/obj/%.o, $(SRC))

all: $(TARGET)
	@echo "compile done..."

$(TARGET): $(OBJ)
	$(AR) $(AR_FLAG) $@ $^

$(ROOT)/obj/%.o: %.c
	$(CC) $(C_FLAG) $(C_DEBUG) $(C_INC) -g -c $^ -o $@

clean:
	$(RM) $(RM_FLAG) $(OBJ) $(TARGET)