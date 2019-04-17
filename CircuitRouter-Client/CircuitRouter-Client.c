// 89522 Pedro Galhardo
// 89544 Tiago Pires

#include <signal.h>
#include <sys/time.h>
#include "../lib/fifo.h"

/**
 * Variáveis globais permitem remover o pipe antes de sair,
 * evitando a acumulação.
 */
int fclient, fshell;
char *fifo_client, *fifo_name, *input, *message, *answer;

void handleExit() {
    FIFO_Close(&fshell);
    FIFO_Unlink(fifo_client);
    free(fifo_client);
    free(input);
    printf("\n\nCircuitRouter-Client ended.\n");
    exit(EXIT_SUCCESS);
}

char* mallocAux(int size){
    char *String = (char*) malloc(sizeof(char)*size);
    if (String == NULL) {
		perror("Error allocating memory");
		exit(EXIT_FAILURE);
	}
    return String;
}

int main(int argc, char** argv){

    /* Definição da resposta ao SIGINT */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleExit;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
  		perror("Error installing handler");
  		exit(EXIT_FAILURE);
  	}

    /* Tipo int tem no máximo 10 algarismos + extensão .pipe */
    fifo_name = mallocAux(16);
    sprintf(fifo_name, "%d", getpid());
    strcat(fifo_name, ".pipe");

    /* Comprimento máximo do nome do fifo. */
    fifo_client =mallocAux(42);
    strcpy(fifo_client,"../CircuitRouter-AdvShell/");
    strcat(fifo_client,fifo_name);
    free(fifo_name);

    if (argc != 2){
        printf("Incorrect input\n");
        free(fifo_client);
        exit(EXIT_FAILURE);
    }

    /* Garantir que o fifo criado anteriormente é removido. */
	FIFO_Unlink(fifo_client);

    FIFO_Make(fifo_client);
    FIFO_Open(&fshell, argv[1], O_WRONLY, -1);

    printf("Welcome to CircuitRouter-Client\n\n");
    while(1) {
        input = mallocAux(98); 
        fgets(input, 98, stdin);

        if (strcmp(input,"\n") == 0){
            free(input);
            continue;
        }

        message = mallocAux(140); /* MAX = 109 caracteres.*/
        answer = mallocAux(23);   /* MAX = 22 caracteres. */

        memset(message, 0, 140);
        memset(answer, 0, 23);
        
        strcpy(message, input);
        strcat(message, " ");
        strcat(message,fifo_client);
        strcat(message, "\n");

        FIFO_Write(&fshell, message,140);
        FIFO_Open(&fclient, fifo_client, O_RDONLY, -1);
        FIFO_Read(&fclient, answer, 23);
        puts(answer);
        FIFO_Close(&fclient);

        free(answer);
        free(message);
        free(input);
    }

    return 0;
}
