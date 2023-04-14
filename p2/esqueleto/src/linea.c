
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
#include <sys/wait.h>

// Modulo principal
int main(int argc, char *argv[])
{
  /*
  Establecimiento semilla de generación de números aleatorio (con id de proceso)
  Coge semáforos y memoria compartida (valor y manejador).
  */

  srand((int)getpid());
  sem_t *mutexespera = get_sem(argv[1]);
  sem_t *telf = get_sem(argv[2]);
  sem_t *linea = get_sem(argv[3]);
  int valorEspera = 0;
  int manejador_espera = obtener_var(argv[4]);

  // Al inicializar la línea se realiza una espera de una llamada (simulación), que será entre 1 y 30 segundos
  printf("Linea [%d] esperando llamada...\n", (int)getpid());
  sleep(rand() % 30 + 1);

  // Aumenta las llamadas en espera
  wait_sem(mutexespera);
  consultar_var(manejador_espera, &valorEspera);
  modificar_var(manejador_espera, ++valorEspera);
  signal_sem(mutexespera);

  // Espera a que un teléfono esté libre, para poder desviar la llamada
  printf("Linea [%d] esperando telefono libre...Nº Llamadas en espera: %d\n", (int)getpid(), valorEspera);
  wait_sem(telf);

  // Lanza la llamada
  printf("Linea [%d] desviando llamada a un telefono...\n", (int)getpid());
  signal_sem(linea);
  return EXIT_SUCCESS;
}