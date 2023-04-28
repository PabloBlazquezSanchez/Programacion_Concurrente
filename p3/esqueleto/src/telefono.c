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
    // TODO
    char buzonLinea[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    srand(pid);

    mqd_t qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);

    while (1)
    {
        printf("Teléfono [%d] en espera...\n", pid);
        mq_receive(qHandlerLlamadas, buzonLinea, 15, NULL);

        printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);

        sleep(rand() % 10 + 10);
        mqd_t qHandlerLinea = mq_open(buzonLinea, O_RDWR);
        mq_send(qHandlerLinea, buffer, TAMANO_MENSAJES+1, 0);
        printf("Teléfono [%d] ha colgado (%s) la llamada. %s\n", pid, FIN_CONVERSACION, buzonLinea);
        mq_close(qHandlerLinea);
    }

    return EXIT_SUCCESS;
}