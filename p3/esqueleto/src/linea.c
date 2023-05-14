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
    char buffer[TAMANO_MENSAJES];

    // Inicia Random
    //Establecimiento semilla de generación de números aleatorio (con id de proceso)
    srand(pid);

    // Verifica los parámetros

    //Si el número de argumentos pasados a la clase linea es < 2
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/linea <cola_linea_llamante>.\n");
        exit(EXIT_FAILURE);
    }

    //Si no se obtiene el nombre del buzon linea
    if (sscanf(argv[1], "%s", buzonLinea) == -1)
    {
        fprintf(stderr, "Error al obtener el nombre del buzón. %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //! sprintf(buzonLinea, "%s", argv[1]); //// O bien este

    //Abrimos dos colas de mensajes para las llamadas y las líneas
    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
    qHandlerLinea = mq_open(buzonLinea, O_RDWR);

    while (1) //La línea está siempre en ejecución, hasta que el manager fuerce su finalización
    {
        //Mensaje de espera
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(rand() % 30 + 1);

        //Línea recibe llamada y envía el mensaje al teléfono para ocuparlo
        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        mq_send(qHandlerLlamadas, buzonLinea, sizeof(buzonLinea), 0);

        //Línea espera fin de conversación
        printf("Linea [%d] esperando fin de conversacion...\n", pid);

        //Línea recibe de teléfono fin de conversación
        mq_receive(qHandlerLinea, buffer, sizeof(buffer), NULL);
        printf("Linea [%d] me ha colgado la llamada (%s) el teléfono %s\n", pid, buzonLinea, buffer);
    }

    return EXIT_SUCCESS;
}