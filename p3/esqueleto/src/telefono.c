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

    // TODO

    // if (argc != 2)
    // {
    //     fprintf(stderr, "Error. Usa: ./exec/telefono <cola_llamadas_a_ser_atendidas>.\n");
    //     exit(EXIT_FAILURE);
    // }

    mqd_t qHandlerLlamadas;

    while (1)
    {
        qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
        printf("Teléfono [%d] en espera...\n", pid);
        mq_receive(qHandlerLlamadas, buzonLinea, TAMANO_MENSAJES, NULL);

        //
        // printf("¿%s?\n", buzonLinea);
        //

        printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);

        sleep(rand() % 10 + 10);
        mqd_t qHandlerLinea = mq_open(buzonLinea, O_RDWR);
        mq_send(qHandlerLinea, buffer, (int)strlen(buffer), 0);
        printf("Teléfono [%d] ha colgado (%s) la llamada. %s\n", pid, FIN_CONVERSACION, buzonLinea);
        mq_close(qHandlerLinea);
        mq_close(qHandlerLlamadas);
    }

    return EXIT_SUCCESS;
}