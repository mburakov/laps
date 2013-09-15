CC=gcc
CFLAGS=-Wall -g3 -O0 -std=c11
LDFLAGS=
LIBS=xcb

CFLAGS+=$(shell pkg-config --cflags $(LIBS))
LDFLAGS+=$(shell pkg-config --libs $(LIBS))
SOURCES=$(wildcard *.c)
RESULT=$(notdir $(CURDIR))
COMPLETION=.clang_complete

all: $(COMPLETION) $(RESULT)

$(COMPLETION):
	echo $(CFLAGS) > $(COMPLETION)

$(RESULT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(RESULT) $(LDFLAGS)

clean:
	rm $(RESULT) $(COMPLETION)

.PHONY: all clean
