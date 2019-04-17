#include "sync.h"

void mutexINIT(pthread_mutex_t *mutex){
    int error;
    error=pthread_mutex_init(mutex,NULL);
    if (error != 0){
        perror("ERROR MUTEX INIT: ");
        exit(EXIT_FAILURE);
    }
}

void mutexLOCK(pthread_mutex_t *mutex){
    int error;
    error=pthread_mutex_lock(mutex);
    if (error != 0){
        perror("ERROR MUTEX LOCK: ");
        exit(EXIT_FAILURE);
    }
}

int mutexTRYLOCK(pthread_mutex_t *mutex){
    int error;
    error=pthread_mutex_trylock(mutex);
    if  (error == EBUSY){
       return 0;   
    }
    else if (error == EINVAL){
        perror("ERROR MUTEX TRYLOCK: ");
        exit(EXIT_FAILURE);
    }
    return 1;
}

void mutexUNLOCK(pthread_mutex_t *mutex){
    int error;
    error=pthread_mutex_unlock(mutex);
    if (error != 0){
        perror("ERROR MUTEX UNLOCK: ");
        exit(EXIT_FAILURE);
    }    
}  

void mutexDESTROY(pthread_mutex_t *mutex){
    int error;
    error=pthread_mutex_destroy(mutex);
    if (error != 0){
        perror("ERROR MUTEX DESTROY: ");
        exit(EXIT_FAILURE);
    }        
}

void pathUNLOCK(vector_t* pointVectorPtr, long pos, grid_t* gridPtr, 
router_solve_arg_t* routerArgPtr) {
    long* gridPointPtr=NULL;

	for(long i = pos; i > 0; i--){
        gridPointPtr = (long*)vector_at(pointVectorPtr, i);
        mutexUNLOCK(&(routerArgPtr->mutexGrid[mutexGridAux(
            gridPointPtr, gridPtr)]));
    }
}

long pathLOCK(vector_t* pointVectorPtr,grid_t* gridPtr,grid_t* myGridPtr,
router_solve_arg_t* routerArgPtr, long *last_pos){
       
        int randNum, mutex_Sucess;
        long n;
        long* gridPointPtr=NULL;

        n = vector_getSize(pointVectorPtr);
        for (int i = 1; i < (n-1); i++) {
            *(last_pos)=i;
            gridPointPtr = (long*)vector_at(pointVectorPtr, i);

            if (mutexTRYLOCK(&(routerArgPtr->mutexGrid[mutexGridAux(
                gridPointPtr, gridPtr)]))){       
                //If point is full, restarts Expansion...
                if(*gridPointPtr == GRID_POINT_FULL)
                    return 0;  
            }                             
            else
                // Backoff
                for (int cnt = 1; cnt < 5; cnt++) {
                    randNum = random()%900+100; // 100 to 1000
                    struct timespec ts = {0, randNum};
                    nanosleep(&ts, NULL);
                    mutex_Sucess = mutexTRYLOCK(&(routerArgPtr->mutexGrid[mutexGridAux(
                        gridPointPtr, gridPtr)]));

                    if (mutex_Sucess) { //Mutex locked
                        if(*gridPointPtr == GRID_POINT_FULL) 
                            return 0;
                        break;
                    }
                    else if (cnt == 4 && !mutex_Sucess){
                        //backoff complete and mutex still not locked
                        *(last_pos)= i-1;//because point with index i was not locked
                        return 0;
                    }  
                } 
        }
        return 1; 
}

long mutexGridAux(long* gridPointPtr, grid_t* gridPtr) {
    coordinate_t point;
   	grid_getPointIndices(gridPtr,gridPointPtr,&point.x,&point.y,&point.z);
    return (point.z* gridPtr->height + point.y) * gridPtr->width + point.x;
}
