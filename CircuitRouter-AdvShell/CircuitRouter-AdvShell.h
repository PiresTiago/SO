#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "../lib/vector.h"
#include "../lib/timer.h"
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    pid_t pid;
    int status;
    TIMER_T start;
	TIMER_T end;
} child_t;

void waitForChild();
void printChildren(vector_t *children);

#endif /* CIRCUITROUTER_SHELL_H */
