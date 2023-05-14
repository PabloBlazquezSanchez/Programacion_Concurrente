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
    char buffer[TAMANO_MENSAJES];

    srand(pid);

    mqd_t qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);

    while (1) //El teléfono está siempre en ejecución, hasta que el manager fuerce su finalización
    {
        //Mensaje de espera
        printf("Teléfono [%d] en espera...\n", pid);

        //Recibimos un mensaje proveniente de un buzón
        mq_receive(qHandlerLlamadas, buzonLinea, sizeof(buzonLinea), NULL);

        //Simulación de la conversación de entre 10 y 20 segundos
        printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);

        sleep(rand() % 10 + 10);
        mqd_t qHandlerLinea = mq_open(buzonLinea, O_RDWR);
        printf("Teléfono [%d] ha %s la llamada. %s\n", pid, FIN_CONVERSACION, buzonLinea);
        sprintf(buffer, "%d", pid);
        mq_send(qHandlerLinea, buffer, sizeof(buffer), 0);
        mq_close(qHandlerLinea);
    }

    return EXIT_SUCCESS;
}