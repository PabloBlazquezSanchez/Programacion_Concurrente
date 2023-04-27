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
  char buffer[TAMANO_MENSAJES + 1];
  char buzonLinea[TAMANO_MENSAJES];

  // Inicia Random
  srand(pid);

  if (argc != 2)
  {
    fprintf(stderr, "Error. Usa: ./exec/telefono <cola_linea_llamadas>.\n");
    exit(EXIT_FAILURE);
  }

  mqd_t qHandlerLlamadas = mq_open(argv[1], O_RDWR);
  // TODO

  while (1)
  {
    printf("Teléfono [%d] en espera...\n", pid);
    mq_receive(qHandlerLlamadas, buzonLinea, strlen(buzonLinea), NULL);
    printf("%s",buzonLinea);
    mqd_t qHandlerLinea=mq_open(buzonLinea,O_RDWR);
    printf("Teléfono [%d] en conversacion de llamada desde Linea: %s\n",pid,buzonLinea);
    sleep(rand() % 10 + 10);
    mq_send(qHandlerLinea, buffer, strlen(buffer), 0);
    printf("Teléfono [%d] ha colgado (%s) la llamada. %s\n",pid,FIN_CONVERSACION,buzonLinea);
    mq_close(qHandlerLinea);
  }
  return EXIT_SUCCESS;
}