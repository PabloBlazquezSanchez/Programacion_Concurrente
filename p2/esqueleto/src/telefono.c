
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

/*
▪ Se inician en espera de una llamada, informando de ello: “Teléfono[UID] en
  espera…”
▪ Cuando reciben una llamada, tienen una conversación variable entre 10..20 segundos
  e informan de ello: “Teléfono[UID] en conversación…”, además en esa misma línea
  informarán del número de llamadas en espera en ese momento:"Nº de llamadas en
  espera: [shm]” y decrementarán el número de llamadas en espera.
▪ Cuando finalicen su llamada, volverán al estado de espera inicial.
▪ En ningún caso finalizarán su ejecución salvo cuando el proceso Manager lo
  considere oportuno.
*/

// Modulo principal
int main(int argc,char *argv[]){


	//TODO: Esquema según especificado en la práctica.

    // Define variables locales



    // Coge semáforos y memoria compartida


    
    // Se pone en estado de libre incrementando el número de teléfonos libres
    while(1){

        // Mensaje de Espera
        printf("Teléfono [%d] en espera...\n",pid);
        

		//TODO: Aquí hay que realizar procesos


        // Mensaje de en conversacion
        printf("Teléfono [%d] en conversacion... Nº Llamadas en espera: %d\n",pid,valorEspera);

        // Espera en conversación
        sleep(rand() % 10 + 10);
        
    }

    return EXIT_SUCCESS;
}