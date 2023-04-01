
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

/*
▪ Se inician y esperan una llamada (Simulado con un rand() entre 1..30 segundos),
  informando de ello: “Linea[UID] esperando llamada”.
▪ Cuando reciben la llamada (Finalización de rand()):
• Incrementarán el contador de llamadas en espera.
• Esperarán a que exista un teléfono libre, informando de ello: “Linea[UID]
  esperando teléfono libre…”, además en esa misma línea informarán del número
  de llamadas en espera en ese momento:"Nº de llamadas en espera: [shm]”.
• Cuando exista un teléfono libre, desviarán la llamada hacia el teléfono,
  informando de ello: “Linea[UID] desviando llamada a un teléfono…” y
  finalizarán su trabajo
*/

// Modulo principal
int main(int argc,char *argv[]){

	//TODO: Esquema especificado en la práctica.
	
	
    // Coge semáforos y memoria compartida


    // Realiza una espera entre 1..60 segundos
    printf("Linea [%d] esperando llamada...\n",pid);
    sleep(rand() % 30 + 1);

    //Aumenta las llamadas en espera


    // Espera telefono libre
    printf("Linea [%d] esperando telefono libre...Nº Llamadas en espera: %d\n",pid,valorEspera);

    // Lanza la llamada
    printf("Linea [%d] desviando llamada a un telefono...\n",pid);


    return EXIT_SUCCESS;
}