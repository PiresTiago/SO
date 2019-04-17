#ifndef FIFO_H
#define FIFO_H 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void FIFO_Make(char* pathname);
void FIFO_Unlink(char*pathname);
void FIFO_Open(int *fd, char* pathname, int oflag1, int oflag2);
void FIFO_Close(int *fd);
void FIFO_Read(int *fd, char* message, int size);
void FIFO_Write(int *fd, char*message, int sizeMessage);

#endif /* FIFO_H */
