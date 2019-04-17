#include "fifo.h"

void FIFO_Make(char* pathname){
    if (mkfifo(pathname, 0666) == -1){
        perror("mkfifo()");
        exit(EXIT_FAILURE);
    }
}

void FIFO_Unlink(char * pathname) {
    int error=unlink(pathname);
    if (errno ==ENOENT); //indica que ficheiro não existe
    else if (error == -1) {
        perror("unlink()");
        exit(EXIT_FAILURE);
    }
}

void FIFO_Open(int * fd, char * pathname, int oflag1, int oflag2) {

    if (oflag2 == -1) 
        do{
            *fd = open(pathname, oflag1);
        }while(*fd == -1 && errno == EINTR );
    else 
        do{
            *fd = open(pathname, oflag1,oflag2);
        }while(*fd == -1 && errno == EINTR );

    if(*fd == -1){
        perror("open()");
        exit(EXIT_FAILURE);
    }
}

void FIFO_Close(int *fd){
    int error = close(*fd);

    while (error == -1 && errno == EINTR){
        error=close(*fd);
    }
    if (error == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
    }
}

void FIFO_Read(int *fd, char* message, int size){
    int error = read(*fd, message, size);
    
    while (error == -1 && errno == EINTR){
        error=read(*fd, message, size);
    }
    if (error == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }
}

void FIFO_Write(int* fd, char*message, int sizeMessage) {

    int error = write(*fd, message, sizeMessage);

    while (error == -1 && errno == EINTR){
        error=write(*fd, message, sizeMessage);
    }
    if (error == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }
}
