#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
#include <sys/wait.h>

void telefono(sem_t *mutex, sem_t *telf, sem_t *linea, int wait_handler, int valorEspera);

// Modulo principal
int main(int argc, char *argv[])
{
  /*
  Establecimiento semilla de generación de números aleatorio (con id de proceso)
  Coge semáforos y memoria compartida (valor y manejador).
  */

  srand((int)getpid());
  sem_t *mutex = get_sem(argv[1]);
  sem_t *telf = get_sem(argv[2]);
  sem_t *linea = get_sem(argv[3]);
  int valor_llamadas = 0;
  int manejador_espera = obtener_var(argv[4]);

  // Lo que declaramos aquí, lo pasamos al metodo del telefono
  telefono(mutex, telf, linea, manejador_espera, valor_llamadas);
  return EXIT_SUCCESS;
}

void telefono(sem_t *mutex, sem_t *telf, sem_t *linea, int wait_handler, int valorEspera)
{
  while (1) // El teléfono está siempre en ejecución, hasta que el manager fuerce su finalización
  {
    // Mensaje de Espera
    printf("Teléfono [%d] en espera...\n", (int)getpid());
    // Se pone en estado de libre incrementando el número de teléfonos libres (disponible para una llamada)
    signal_sem(telf);

    // Esperamos a que se desvíe al teléfono una llamada
    wait_sem(linea);

    // Decrementamos el número de llamadas en espera
    wait_sem(mutex);
    consultar_var(wait_handler, &valorEspera);
    modificar_var(wait_handler, --valorEspera);
    signal_sem(mutex);

    // Simulación de la conversación de entre 10 y 20 segundos
    printf("Teléfono [%d] en conversacion... Nº Llamadas en espera: %d\n", (int)getpid(), valorEspera);
    sleep(rand() % 10 + 10);
  }
}