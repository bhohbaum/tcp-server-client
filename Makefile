CC = gcc
BASE_CFLAGS = -O3 -Wall
DEBUG_CFLAGS = -g
DEPS = tcpserver.c tcpclient.c
EXECS = tcpserver tcpclient
OBJ = tcpserver.o tcpclient.o 

ifdef DEBUG
CFLAGS = $(BASE_CFLAGS) $(DEBUG_CFLAGS)
else
CFLAGS = $(BASE_CFLAGS)
endif

all: $(EXECS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECS): $(OBJ)
	$(CC) -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJ) $(EXECS)
