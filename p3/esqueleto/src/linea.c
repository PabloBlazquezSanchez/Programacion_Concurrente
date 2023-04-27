
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include <definitions.h>

/*
* Recibirá como parámetro su cola de mensajes para el proceso de notificación de
* teléfonos a líneas (buzon_linea_n)

*▪Se inician y esperan una llamada (Simulado con un rand() entre 1..30 segundos),
* informando de ello: “Linea[UID] esperando llamada”.

*▪Cuando reciben la llamada (Finalización de rand()):

?•Enviará un mensaje a la cola de llamadas para ser atendidas.

?•Esperarán la notificación desde un teléfono, de que la llamada ha finalizado.

• Notificarán la recepción del teléfono y volverán al estado de espera de llamada.

!▪En ningún caso finalizarán su ejecución salvo cuando el proceso Manager lo
! considere oportuno.
*/

/*
Se deben crear 10 de qHandlerLinea[i]
*/

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
    sprintf(buzonLinea, "%s", argv[1]);
    // printf("%s\n", buzonLinea);
    // Inicializar la línea

    // TODO
    while (1)
    {
        qHandlerLinea = mq_open(buzonLinea, O_RDWR);
        printf("Linea [%d] esperando llamada...\n", pid);
        sleep(rand() % 30 + 1);
        printf("Linea [%d] recibida llamada (%s)...\n", pid, buzonLinea);
        qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_RDWR);
        mq_send(qHandlerLlamadas, buzonLinea, strlen(buzonLinea), 0);
        printf("Linea [%d] esperando fin de conversacion...\n",pid);
        mq_receive(qHandlerLinea, buffer, strlen(buffer), NULL);
        mq_close(qHandlerLlamadas);
        mq_close(qHandlerLinea);
    }
    return EXIT_SUCCESS;
}