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
    int pid = (int)getpid(); //Variable local entera que recoge el pid del proceso
    char buzonLinea[TAMANO_MENSAJES]; //Cadena de caracteres con un tamaño correspondiente a TAMANO_MENSAJES
    char buffer[TAMANO_MENSAJES]; //Cadena de caracteres con un tamaño correspondiente a TAMANO_MENSAJES

    //Establecimiento semilla de generación de números aleatorio (con id de proceso)
    srand(pid);

    //Creamos y abrimos una cola de mensaje para las llamadas
    mqd_t qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);

    while (1) //El teléfono está siempre en ejecución, hasta que el manager fuerce su finalización
    {
        //Mensaje de espera a una llamada
        printf("Teléfono [%d] en espera...\n", pid);
        //Espera a recibir un mensaje proveniente de un buzón de la línea a atender
        mq_receive(qHandlerLlamadas, buzonLinea, sizeof(buzonLinea), NULL);

        //Simulación de la conversación de entre 10 y 20 segundos
        printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);
        sleep(rand() % 10 + 10);

        //Mensaje de finalización del teléfono usado y envío de mensaje a la línea
        printf("Teléfono [%d] ha %s la llamada. %s\n", pid, FIN_CONVERSACION, buzonLinea);
        //Abrimos una cola de mensaje de la linea atendida
        mqd_t qHandlerLinea = mq_open(buzonLinea, O_RDWR);
        sprintf(buffer, "%d", pid);
        mq_send(qHandlerLinea, buffer, sizeof(buffer), 0);

        //Cerramos la cola de mensajes abierta anteriormente
        mq_close(qHandlerLinea);
    }

    return EXIT_SUCCESS;
}