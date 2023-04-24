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

▪ En ningún caso finalizarán su ejecución salvo cuando el proceso Manager lo
  considere oportuno.
*/

// Modulo principal
int main(int argc, char *argv[])
{

    // Define variables locales
    int pid = getpid();
    //? Me interesa tener el buzon de las líneas, para saber qué linea es cada llamada

    // TODO

    srand(pid);

    // TODO

    while(1){
        printf("Teléfono [%d] en espera...\n", (int)getpid());
        // mq_send(buzones[restante]...¿qHandlerLinea?, msg);
        // mq_receive()

    }

    return EXIT_SUCCESS;
}