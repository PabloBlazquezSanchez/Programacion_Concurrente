
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
#include<sys/wait.h>

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
int main(int argc, char *argv[])
{
  int valorEspera = 0;
  srand((int)getpid());
  // Coge semáforos y memoria compartida
  sem_t *mutexespera = get_sem(/*id_mutexespera*/ argv[1]);
  sem_t *telf = get_sem(/*id_telefono*/ argv[2]);
  sem_t *linea = get_sem(/*id_linea*/ argv[3]);
  int manejador_espera = obtener_var(/*LLAMADAESPERA*/ argv[4]);
  // TODO: Esquema especificado en la práctica.
  // Realiza una espera entre 1..30 segundos
  printf("Linea [%d] esperando llamada...\n", (int)getpid());
  sleep(rand() % 30 + 1);

  // Aumenta las llamadas en espera
  wait_sem(mutexespera);
  consultar_var(manejador_espera, &valorEspera);
  modificar_var(manejador_espera, ++valorEspera);
  signal_sem(mutexespera);

  // Espera telefono libre
  printf("Linea [%d] esperando telefono libre...Nº Llamadas en espera: %d\n", (int)getpid(), valorEspera);

  // Lanza la llamada
  printf("Linea [%d] desviando llamada a un telefono...\n", (int)getpid());
  signal_sem(telf);
  wait_sem(linea);
  return EXIT_SUCCESS;
}