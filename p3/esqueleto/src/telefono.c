#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include <definitions.h>

/*
▪ Se inician en espera de una llamada, informando de ello: “Teléfono[UID] en
  espera…”

▪ Cuando reciben una llamada, tienen una conversación variable entre 10..20 segundos
  e informan de ello: “Teléfono[UID] en conversación de llamada desde la línea:
  [buzon_linea_n]…”.

▪ Cuando finalicen su conversación, notificarán a la línea este suceso y mostrarán un
  mensaje: Teléfono [%d] ha colgado la llamada. [buzon_linea_n]

▪ Volverán a su estado inicial de espera de llamada.

!▪En ningún caso finalizarán su ejecución salvo cuando el proceso Manager lo
! considere oportuno.
*/

// Modulo principal
int main(int argc, char *argv[])
{

  // Define variables locales
  int pid = (int)getpid();
  mqd_t qHandlerLinea;

  char buffer[TAMANO_MENSAJES + 1];
  char buzonLinea[TAMANO_MENSAJES];

  // Inicia Random
  srand(pid);

  if (argc != 2)
  {
    fprintf(stderr, "Error. Usa: ./exec/telefono <cola_linea_llamadas>.\n");
    exit(EXIT_FAILURE);
  }

  mqd_t qHandlerLlamadas = mq_open(argv[1],O_RDWR);
  // TODO

  while (1)
  {
    qHandlerLinea=mq_open(buzonLinea,O_RDWR);
    printf("Teléfono [%d] en espera...\n", pid);
    // mq_receive() SIEMPRE Y CUANDO HAYAN COSAS EN LLAMADA
    mq_receive(qHandlerLlamadas, buzonLinea, sizeof(buzonLinea), 0);
    mq_send(qHandlerLinea, buffer, sizeof(buffer), 0);
  // Teléfono [49728] en conversacion de llamada desde Linea: /buzon_linea_6
    printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n", pid, buzonLinea);
    sleep(rand() % 10 + 10);
    // notificar fin de llamada
    printf("Teléfono [%d] ha %s la llamada. %s\n", pid, FIN_CONVERSACION, buzonLinea);
    // mq_send(buzones[restante]...¿qHandlerLinea?, msg);
    mq_close(buzonLinea);
  }

  return EXIT_SUCCESS;
}