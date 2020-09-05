# Makefile template for C/C++

.PHONY: all clean

# path
ROOT = ../..
TARGET_PATH := $(ROOT)/bin
INCLUDE_PATH := $(ROOT)/include
OBJ_PATH := $(ROOT)/obj
SRC_PATH := .
STATIC_LIB_PATH := $(ROOT)/lib

# name
TARGET_NAME := xref
STATIC_LIB_NAME := cii

# tools
CC := gcc
C_STD := c99
C_INC := -I $(INCLUDE_PATH)
C_LIB := -L $(STATIC_LIB_PATH) -l$(STATIC_LIB_NAME)
C_DEGUG := -g
C_FLAG := -Wall -std=$(C_STD)

C_FLAG += $(C_INC)
C_FLAG += $(C_LIB)
C_FLAG += $(C_DEGUG)

# AR := ar
# AR_FLAG := rcsv

RM := rm
RM_FLAG := -rf

# files
TARGET := $(TARGET_PATH)/$(TARGET_NAME)
SRC := $(wildcard $(SRC_PATH)/*.c)

# rules
all: $(TARGET)
	@echo "compile done..."

$(TARGET): $(SRC)
	$(CC) $^ $(C_FLAG) -o $@

clean: 
	$(RM) $(RM_FLAG) $(TARGET)

