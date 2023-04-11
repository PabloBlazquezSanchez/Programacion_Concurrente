
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
#include<sys/wait.h>

/*
* Se inician en espera de una llamada, informando de ello: “Teléfono[UID] en espera…”
▪ Cuando reciben una llamada, tienen una conversación variable entre 10..20 segundos
  e informan de ello: “Teléfono[UID] en conversación…”, además en esa misma línea
  informarán del número de llamadas en espera en ese momento:"Nº de llamadas en
  espera: [shm]” y decrementarán el número de llamadas en espera.
▪ Cuando finalicen su llamada, volverán al estado de espera inicial.
▪ En ningún caso finalizarán su ejecución salvo cuando el proceso Manager lo
  considere oportuno.
*/

void telefono(sem_t *mutex, sem_t *telf, sem_t *linea, int wait_handler, int valorEspera);

// Modulo principal
int main(int argc, char *argv[])
{
  // TODO: Esquema según especificado en la práctica.

  // Define variables locales. Coge semáforos y memoria compartida
  int valor_llamadas = 0;
  srand((int)getpid());
  sem_t *mutex = get_sem(/*id_mutexespera*/ argv[1]);
  sem_t *telf = get_sem(/*id_telefono*/ argv[2]);
  sem_t *linea = get_sem(/*id_linea*/ argv[3]);
  int manejador_espera = obtener_var(/*LLAMADAESPERA*/ argv[4]);

  //! Lo que declaremos aquí, lo pasamos todo al metodo del telefono
  telefono(mutex, telf, linea, manejador_espera, valor_llamadas);
  return EXIT_SUCCESS;
}

void telefono(sem_t *mutex, sem_t *telf, sem_t *linea, int wait_handler, int valorEspera)
{
  // Se pone en estado de libre incrementando el número de teléfonos libres
  while (1) //! BUCLE INFINITO. SÓLO ACABA SU EJECUCIÓN POR EL CIERRE DEL MANAGER.
  {
    // Mensaje de Espera
    printf("Teléfono [%d] en espera...\n", (int)getpid());

    signal_sem(telf);
    wait_sem(linea);

    wait_sem(mutex);
    consultar_var(wait_handler, &valorEspera);
    modificar_var(wait_handler, --valorEspera); //! Presupongo que cuando se coge el teléfono, automáticamente ya no está en espera. Decremento una unidad
    signal_sem(mutex);
    // Mensaje de EN CONVERSACION
    printf("Teléfono [%d] en conversacion... Nº Llamadas en espera: %d\n", (int)getpid(), valorEspera);
    // Espera en conversación
    sleep(rand() % 10 + 10);
    // signal_sem del semáforo de la línea para indicar que yá está desocupada?
  }
}