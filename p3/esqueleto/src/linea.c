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
    //char buzonLinea[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    // Inicia Random
    srand(pid);

    // TODO

    // Verifica los parámetros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/linea <cola_linea_llamante>.\n");
        exit(EXIT_FAILURE);
    }

    char *buzonLinea = argv[1];

    while (1)
    {
        qHandlerLinea = mq_open(argv[1], O_RDWR);
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(rand() % 30 + 1);

        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
        mq_send(qHandlerLlamadas, buzonLinea, (int)strlen(buzonLinea), 0);
        printf("Linea [%d] esperando fin de conversacion...\n", pid);
        mq_receive(qHandlerLinea, buffer, (int)strlen(buffer), NULL);
        mq_close(qHandlerLlamadas);
        mq_close(qHandlerLinea);
    }

    return EXIT_SUCCESS;
}