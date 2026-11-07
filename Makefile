# Override with e.g. make CC=gcc-15
ifeq ($(origin CC), default)
CC := gcc
endif
CPPFLAGS += -Ivendor/fenster -D_POSIX_C_SOURCE=200809L
CFLAGS ?= -std=c11 -Wall -Wextra -g -O0
BUILD_DIR := build
TARGET := $(BUILD_DIR)/pixel-game
SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h) vendor/fenster/fenster.h

ifeq ($(OS),Windows_NT)
TARGET := $(TARGET).exe
LDLIBS += -lgdi32
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS += -framework Cocoa
else
LDLIBS += -lX11
endif
endif

.PHONY: all run clean
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
