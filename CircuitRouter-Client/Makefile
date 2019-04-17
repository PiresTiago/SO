# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2018-19

SOURCES = CircuitRouter-Client.c
SOURCES+= ../lib/fifo.c
OBJS = $(SOURCES:%.c=%.o)
CC   = gcc
CFLAGS =-g -Wall -std=gnu99 -I../
LDFLAGS=-lm
TARGET = CircuitRouter-Client

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)

CircuitRouter-Client.o: CircuitRouter-Client.c ../lib/fifo.h
../lib/fifo.o: ../lib/fifo.c ../lib/fifo.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)
