# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2018-19

SOURCES = router.c maze.c grid.c coordinate.c CircuitRouter-ParSolver.c sync.c
SOURCES+= ../lib/vector.c ../lib/queue.c ../lib/list.c ../lib/pair.c
OBJS = $(SOURCES:%.c=%.o)
CC   = @gcc
CFLAGS =-Wall -g -std=gnu99 -pthread -I../
LDFLAGS=-lm
TARGET = CircuitRouter-ParSolver

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo Building...
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS) 
	@echo CircuitRouter-ParSolver: Build Succeeded. 

CircuitRouter-ParSolver.o: CircuitRouter-ParSolver.c sync.h maze.h router.h ../lib/list.h ../lib/timer.h ../lib/types.h
router.o: router.c router.h coordinate.h grid.h sync.h ../lib/queue.h ../lib/vector.h
maze.o: maze.c maze.h coordinate.h grid.h ../lib/list.h ../lib/queue.h ../lib/pair.h ../lib/types.h ../lib/vector.h
grid.o: grid.c grid.h coordinate.h router.h ../lib/types.h ../lib/vector.h
coordinate.o: coordinate.c coordinate.h ../lib/pair.h ../lib/types.h
sync.o: sync.c sync.h
../lib/vector.o: ../lib/vector.c ../lib/vector.h ../lib/types.h ../lib/utility.h
../lib/queue.o: ../lib/queue.c ../lib/queue.h ../lib/types.h
../lib/list.o: ../lib/list.c ../lib/list.h ../lib/types.h
../lib/pair.o: ../lib/pair.c ../lib/pair.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	@(rm -f $(OBJS) $(TARGET))
	@echo CircuitRouter-ParSolver: All clear.




