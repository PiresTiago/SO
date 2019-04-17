#ifndef SYNC_H
#define SYNC_H 1

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "../lib/vector.h"
#include "grid.h"
#include "coordinate.h"
#include "router.h"

void mutexINIT(pthread_mutex_t *mutex);
void mutexLOCK(pthread_mutex_t *mutex);
int mutexTRYLOCK(pthread_mutex_t *mutex);
void mutexUNLOCK(pthread_mutex_t *mutex);
void mutexDESTROY(pthread_mutex_t *mutex);
void pathUNLOCK(vector_t* pointVectorPtr, long pos, grid_t* gridPtr, 
router_solve_arg_t* routerArgPtr);
long pathLOCK(vector_t* pointVectorPtr,grid_t* gridPtr,grid_t* myGridPtr,
router_solve_arg_t* routerArgPtr,long *last_pos);
long mutexGridAux(long* gridPointPtr, grid_t* gridPtr);
#endif
