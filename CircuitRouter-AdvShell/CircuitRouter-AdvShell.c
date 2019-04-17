// 89522 Pedro Galhardo
// 89544 Tiago Pires

#include "../lib/fifo.h"
#include "../lib/commandlinereader.h"
#include "../lib/vector.h"
#include "CircuitRouter-AdvShell.h"
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"

#define MAXARGS 100 
#define BUFFER_SIZE 100

/**
 * Variáveis Globais
 * -----------------
 */
vector_t *children;
int runningChildren = 0;

/*
 * Signal Handler
 * --------------
 * Determina que processo filho terminou e regista a informação em falta.
 */
void waitForChild() {
	int status;
	long size;
	pid_t pid;
	TIMER_T endTime;
	child_t *child;

	/* A opção WNOHANG não permite que o handler fique bloqueado. */
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		TIMER_READ(endTime);
    	size = vector_getSize(children);

    	for (int i = 0; i < size; i++) {
			child = vector_at(children, i);
	    	if (child != NULL && child->pid == pid) {
	    		/* Filho correspondente encontrado;
				 * preenche a informação em falta. */
	    		child->end = endTime;
	    		child->status = status;
	    		runningChildren--;
	    		break;
	    	}
	    }
	}
}

/*
 * printChildren
 * -------------
 * Percorre o vector de filhos e apresenta as suas informações.
 */
void printChildren(vector_t *children) {
    for (int i = 0; i < vector_getSize(children); ++i) {
        child_t *child = vector_at(children, i);
        if (child->pid != -1) {
            const char* ret = "NOK";
            if (WIFEXITED(child->status) && WEXITSTATUS(child->status) == 0) {
                ret = "OK";
            }
            printf("CHILD EXITED (PID=%d; return %s; %.0f s)\n",
            	child->pid, ret, TIMER_DIFF_SECONDS(child->start, child->end));
        }
    }
    puts("END.");
}

char* mallocAux(int size){
    char *String = (char*) malloc(sizeof(char)*size);
    if (String == NULL) {
		perror("Error allocating memory");
		exit(EXIT_FAILURE);
	}
    return String;
}

int main (int argc, char** argv) {
    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    int MAXCHILDREN = -1;
    TIMER_T startTime;

    /* Definição da resposta ao SIGCHLD */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = waitForChild;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
  		perror("Error installing handler");
  		exit(EXIT_FAILURE);
  	}

	/* Definição da máscara */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGCHLD);

    if(argv[1] != NULL){
        MAXCHILDREN = atoi(argv[1]);
    }

    children = vector_alloc(MAXCHILDREN);

    fd_set rfds;
    int retval;

    int fclient, fshell;
    char * fifo_shell = mallocAux(28);

    strcpy(fifo_shell, "CircuitRouter-AdvShell.pipe");

	/* Garantir que o fifo(pipe com nome) criado anteriormente é removido. */
	FIFO_Unlink(fifo_shell);
    FIFO_Make(fifo_shell);
    FIFO_Open(&fshell, fifo_shell, O_RDWR, O_NONBLOCK);

    printf("Welcome to CircuitRouter-AdvShell\n\n");
	
    while (1) {
        int numArgs;
        char * input = mallocAux(140);

        FD_ZERO(&rfds);
        FD_SET(fshell, &rfds);
        FD_SET(0, &rfds);

        retval = select(fshell+1, &rfds, NULL, NULL, NULL);

        if (retval == -1) {
        	if (errno == EINTR){
                free(input);
				continue;
            }
            perror("select()");
            exit(EXIT_FAILURE);
        }

        /* Ler do fifo. */
        if (FD_ISSET(fshell, &rfds)) {
            FIFO_Read(&fshell, input, 140);
            numArgs = readLineArguments(args, MAXARGS+1, input, BUFFER_SIZE, 0);

			if (numArgs > 0 && strcmp(args[0], "run") != 0) {
                FIFO_Open(&fclient, args[numArgs-1], O_WRONLY, -1);
                FIFO_Write(&fclient, "Command not supported\n",23);
                FIFO_Close(&fclient);
                free(input);
                continue;
            }
        }

        /* Ler do stdin. */
        else if (FD_ISSET(0, &rfds)) {
            numArgs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE, 1);
        }

        /* EOF (end of file) do stdin ou comando "sair" */
        if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0))) {
            printf("CircuitRouter-AdvShell will exit.\n--\n");

			/* Aguarda a receção de signals até runningChildren == 0,
			 * evitando espera ativa.
			 */
            while (runningChildren > 0) {
				pause();
			}

            printChildren(children);
            printf("--\nCircuitRouter-AdvShell ended.\n");
            FIFO_Unlink(fifo_shell);
            free(input);
            free(fifo_shell);
            break;
        }

        else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0) {

			/* Caso MAXCHILDREN != -1 (sem limite),
			 * aguarda a receção de signals até runningChildren < MAXCHILDREN,
			 * evitando espera ativa.
			 */
            while (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN) {
				pause();
			}

            int pid = fork();

            if (pid < 0) {
                perror("Failed to create new process.");
                exit(EXIT_FAILURE);
            }

            if (pid > 0) {
                TIMER_READ(startTime);

                child_t *child = malloc(sizeof(child_t));
		        if (child == NULL) {
		            perror("Error allocating memory");
		            exit(EXIT_FAILURE);
		        }

		        child->pid = pid;
		        child->start = startTime;

		        /* Bloqueia SIGCHLD signals. */
		        sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);

		        vector_pushBack(children, child);
		        runningChildren++;

		        /* Permite SIGCHLD signals. */
		        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);

                printf("%s: background child started with PID %d.\n", COMMAND_RUN, pid);
                free(input);
        		continue;
            }

            else {
                char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";

                //input veio do fifo
                if (numArgs == 3) {
                    char * newArgs[4] = {seqsolver, args[1], args[2], NULL};
                    execv(seqsolver, newArgs);
					/* Não deveria chegar aqui. */
                    perror("Error while executing child process");
                    exit(EXIT_FAILURE);
                }
                //input veio do stdin
                else {
                    char *newArgs[3] = {seqsolver, args[1], NULL};
                    execv(seqsolver, newArgs);
					/* Não deveria chegar aqui. */
                    perror("Error while executing child process");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else if (numArgs == 0){
            free(input);
            /* Nenhum argumento; ignora e volta a pedir */
            continue;
        }
        else
            printf("Command not supported. Try again.\n");
        free(input);
    }

	int size = vector_getSize(children);
    for (int i = 0; i < size; i++) {
        free(vector_at(children, i));
    }
    vector_free(children);
    return EXIT_SUCCESS;
}
