# word frequency

.PHONY: clean all

ROOT := ../..
INCLUDE_PATH := $(ROOT)/include
LIB_PATH := $(ROOT)/src
LIB_NAME := cii
TARGET := wf

CC := gcc
C_FLAG := -Wall -std=c99
C_INC_PATH := -I $(INCLUDE_PATH)
C_LIB := -l$(LIB_NAME)
C_LIB_PATH := -L $(LIB_PATH)

RM := rm
RM_FLAG := -rf

SRC := wf.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(C_FLAG) -g $^ $(C_LIB_PATH) $(C_LIB) $(C_INC_PATH) -o $@

clean:
	$(RM) $(RM_FLAG) $(TARGET)