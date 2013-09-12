CC=gcc
CFLAGS=-c -Wall -g3 -O0 -std=c11
LDFLAGS=
LIBS=x11

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
RESULT=$(notdir $(CURDIR))
COMPLETION=.clang_complete

$(COMPLETION):
	echo $(CFLAGS) > $(COMPLETION)

all: $(COMPLETION) $(SOURCES) $(RESULT)

$(RESULT): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(RESULT) $(COMPLETION)

.PHONY: all clean
