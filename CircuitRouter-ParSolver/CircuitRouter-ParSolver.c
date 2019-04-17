/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This code is an adaptation of the Lee algorithm's implementation originally
 * included in the STAMP Benchmark
 * by Stanford University.
 *
 * The original copyright notice is included below.
 *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * Unless otherwise noted, the following license applies to STAMP files:
 *
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 *
 * CircuitRouter-ParSolver.c
 *
 * =============================================================================
 */

#include <pthread.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/list.h"
#include "sync.h"
#include "maze.h"
#include "router.h"
#include <string.h>
#include "../lib/timer.h"
#include "../lib/types.h"

enum param_types {
    PARAM_BENDCOST = (unsigned char)'b',
    PARAM_XCOST    = (unsigned char)'x',
    PARAM_YCOST    = (unsigned char)'y',
    PARAM_ZCOST    = (unsigned char)'z',
};

enum param_defaults {
    PARAM_DEFAULT_BENDCOST = 1,
    PARAM_DEFAULT_XCOST    = 1,
    PARAM_DEFAULT_YCOST    = 1,
    PARAM_DEFAULT_ZCOST    = 2,
};

bool_t global_doPrint = FALSE;
char* global_inputFile = NULL;
char* global_resFile = NULL;
long global_params[256]; /* 256 = ascii limit */

int numThreads;
long grid_size;
pthread_mutex_t mutexQueue;
pthread_mutex_t mutexListIns;
pthread_mutex_t *mutexGrid;


/* =============================================================================
 * displayUsage
 * =============================================================================
 */

static void displayUsage (const char* appName){
    printf("Usage: %s [options]\n", appName);
    puts("\nOptions:                            (defaults)\n");
    printf("    b <INT>    [b]end cost          (%i)\n", PARAM_DEFAULT_BENDCOST);
    printf("    x <UINT>   [x] movement cost    (%i)\n", PARAM_DEFAULT_XCOST);
    printf("    y <UINT>   [y] movement cost    (%i)\n", PARAM_DEFAULT_YCOST);
    printf("    z <UINT>   [z] movement cost    (%i)\n", PARAM_DEFAULT_ZCOST);
    printf("    h          [h]elp message       (false)\n");
    exit(1);
}

/* =============================================================================
 * setDefaultParams
 * =============================================================================
 */

static void setDefaultParams (){
    global_params[PARAM_BENDCOST] = PARAM_DEFAULT_BENDCOST;
    global_params[PARAM_XCOST]    = PARAM_DEFAULT_XCOST;
    global_params[PARAM_YCOST]    = PARAM_DEFAULT_YCOST;
    global_params[PARAM_ZCOST]    = PARAM_DEFAULT_ZCOST;
}

/* =============================================================================
 * new parseArgs
 * =============================================================================
 */
static void parseArgs (long argc, char* const argv[]){
	long i;
	long opt;
	opterr = 0;
	
	setDefaultParams();
	global_doPrint = TRUE;
	while ((opt = getopt(argc, argv, "hb:x:y:z:t:")) != -1) {
		switch (opt) {
			case 'b':
			case 'x':
			case 'y':
			case 'z':
				global_params[(unsigned char)opt] = atol(optarg);
				break;
            case 't':
                if(atol(optarg)>0){
                    numThreads=atol(optarg);
                    break;
                }
                else{
                    printf("Invalid Task number\n"); //PARA IMPRIMIR ERRO 'E SO ISTO?
                    exit(EXIT_FAILURE);
                }
			case '?':
			case 'h':
			default:
				opterr++;
				break;
		}
	}
	// Primeiro argumento que não é uma flag
	global_inputFile = argv[optind];

	// Apenas um permitido
	
	for (i = optind+1 ; i < argc; i++) {
		fprintf(stderr, "Non-option argument: %s\n", argv[i]);
		opterr++;
	}
	
	if (opterr)
		displayUsage(argv[0]);
}
 
void printNumThreads(void* numThreads){
    printf("Tenho %d tarefas irmãs\n",*(int*)numThreads);
}
/* =============================================================================
 * main
 * =============================================================================
 */

int main(int argc, char** argv){
    /*
     * Initialization
     */
    int pthread_error;
    parseArgs(argc, (char** const)argv);
	
    //launch threads
    pthread_t tids[numThreads+1];

    maze_t* mazePtr = maze_alloc();
    assert(mazePtr);
    
    long numPathToRoute = maze_read(mazePtr, global_inputFile);
    
    router_t* routerPtr = router_alloc(global_params[PARAM_XCOST],
                                       global_params[PARAM_YCOST],
                                       global_params[PARAM_ZCOST],
                                       global_params[PARAM_BENDCOST]);
    assert(routerPtr);
    list_t* pathVectorListPtr = list_alloc(NULL);
    assert(pathVectorListPtr);

    grid_size=mazePtr->gridPtr->width* mazePtr->gridPtr->height*mazePtr->gridPtr->depth;
    if(!(mutexGrid=(pthread_mutex_t *) malloc(sizeof(pthread_mutex_t)*grid_size))){
        perror("ERROR MALLOC: ");
        exit(EXIT_FAILURE);
    }
    
    //Mutex Initialization
    for (int i = 0; i<grid_size; i++) 
    	mutexINIT(&mutexGrid[i]);
    mutexINIT(&mutexQueue);
    mutexINIT(&mutexListIns);

    router_solve_arg_t routerArg = {routerPtr, mazePtr, pathVectorListPtr, 
    	&mutexQueue, &mutexListIns,mutexGrid};

    TIMER_T startTime;
    TIMER_READ(startTime);
    for (int i=0; i<numThreads;i++){
        pthread_error = pthread_create(&tids[i],NULL,(void*) router_solve,(void *)&routerArg);
        if (pthread_error != 0){
            perror("ERROR PTHREAD CREATE: ");
            exit(EXIT_FAILURE);        
        }
    }
    pthread_create(&tids[numThreads],NULL,(void*) printNumThreads,(void*)&numThreads);
    for (int i=0;i<numThreads+1;i++){
        pthread_error = pthread_join(tids[i],NULL);
        if (pthread_error != 0){
            perror("ERROR PTHREAD JOIN: ");
            exit(EXIT_FAILURE);        
        }
    }
    TIMER_T stopTime;
    TIMER_READ(stopTime);

    //Mutex Destruction
    mutexDESTROY(&mutexListIns);
    mutexDESTROY(&mutexQueue);
    for (int i = 0; i<grid_size; i++) 
    	mutexDESTROY(&mutexGrid[i]);
    free(mutexGrid);
    
    long numPathRouted = 0;
    list_iter_t it;
    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it,
                                                            pathVectorListPtr);
        numPathRouted += vector_getSize(pathVectorPtr);
	}
    
    if(!(global_resFile = malloc(sizeof(char)*(strlen(global_inputFile)+9)))){
        perror("ERROR MALLOC: ");
        exit(EXIT_FAILURE);
    }
    
    strcpy(global_resFile, global_inputFile);
    strcat(global_resFile, ".res");

    FILE *fp = fopen(global_resFile,"a");
    
    fprintf(fp, "Paths routed    = %li\n", numPathRouted);
    fprintf(fp, "Elapsed time    = %f seconds\n", TIMER_DIFF_SECONDS(startTime,
                                                                stopTime));
    
    /*
     * Check solution and clean up
     */
    assert(numPathRouted <= numPathToRoute);
    bool_t status = maze_checkPaths(mazePtr, pathVectorListPtr,
                                    global_doPrint, global_inputFile);
    assert(status == TRUE);

    fprintf(fp, "Verification passed.\n");
    fclose(fp);
    free(global_resFile);
    maze_free(mazePtr);
    router_free(routerPtr);

    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it,
                                                            pathVectorListPtr);
        vector_t* v;
        while((v = vector_popBack(pathVectorPtr))) {
            // v stores pointers to longs stored elsewhere;
            // no need to free them here
            vector_free(v);
        }
        vector_free(pathVectorPtr);
    }
    list_free(pathVectorListPtr);

    exit(0);
}
/* =============================================================================
 *
 * End of CircuitRouter-ParSolver.c
 *
 * =============================================================================
 */
