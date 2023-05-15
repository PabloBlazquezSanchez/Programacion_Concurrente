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

    int pid = (int)getpid(); //Variable local entera que recoge el pid del proceso
    mqd_t qHandlerLlamadas; //Cola de mensajes para llamadas
    mqd_t qHandlerLinea; //Cola de mensajes para línea
    char buzonLinea[TAMANO_MENSAJES]; //Cadena de caracteres con un tamaño correspondiente a TAMANO_MENSAJES
    char buffer[TAMANO_MENSAJES]; //Cadena de caracteres con un tamaño correspondiente a TAMANO_MENSAJES

    //Establecimiento semilla de generación de números aleatorio (con id de proceso)
    srand(pid);

    //Verifica si el número de argumentos pasados a la clase linea es < 2
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/linea <cola_linea_llamante>.\n");
        exit(EXIT_FAILURE);
    }

    //Si obtención del nombre del buzon linea sale mal
    if (sscanf(argv[1], "%s", buzonLinea/*Siempre se hace*/) == -1/*No siempre pasa*/)
    {
        fprintf(stderr, "Error al obtener el nombre del buzón. %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //Abrimos dos colas de mensajes para las llamadas y las líneas
    qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
    qHandlerLinea = mq_open(buzonLinea, O_RDWR);

    while (1) //La línea está siempre en ejecución, hasta que el manager fuerce su finalización
    {
        //Mensaje de espera
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(rand() % 30 + 1);

        //Línea recibe llamada y envía el mensaje con el nombre del buzón al teléfono para ocuparlo
        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        mq_send(qHandlerLlamadas, buzonLinea, sizeof(buzonLinea), 0);

        //Línea espera fin de conversación del teléfono
        printf("Linea [%d] esperando fin de conversacion...\n", pid);

        //Linea espera al teléfono
        mq_receive(qHandlerLinea, buffer, sizeof(buffer), NULL);

        //Línea recibe de teléfono fin de teléfono
        printf("Linea [%d] me ha colgado la llamada (%s) el teléfono %s\n", pid, buzonLinea, buffer);
    }

    return EXIT_SUCCESS;
}