#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include <definitions.h>

// Modulo principal
int main(int argc, char *argv[])
{

    // Define variables locales
    int pid = (int)getpid();
    mqd_t qHandlerLlamadas;
    mqd_t qHandlerLinea;
    char buzonLinea[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    // Inicia Random
    srand(pid);

    // Verifica los parámetros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/linea <cola_linea_llamante>.\n");
        exit(EXIT_FAILURE);
    }

    if (sscanf(argv[1], "%s", buzonLinea) == -1)
    {
        fprintf(stderr, "Error al obtener el nombre del buzón. %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // sprintf(buzonLinea,"%s",argv[1]); //// O bien este
    
    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
    qHandlerLinea = mq_open(argv[1], O_RDWR);
    
    while (1)
    {
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(rand() % 30 + 1);

        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        mq_send(qHandlerLlamadas, buzonLinea, 15, 0);
        printf("Linea [%d] esperando fin de conversacion...\n", pid);
        mq_receive(qHandlerLinea, buffer, TAMANO_MENSAJES+1, NULL);
    }

    return EXIT_SUCCESS;
}