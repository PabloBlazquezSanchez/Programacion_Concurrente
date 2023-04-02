#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

/*
* Recibirá dos argumentos: <nº teléfonos> <nº líneas>
* Inicializará los semáforos y la memoria compartida para almacenar el número de llamadas en espera.
▪ Lanzará el número de procesos teléfono y procesos línea especificado en los
  argumentos.
▪ Realizará el apagado controlado de Obelix. Para ello, esperará la finalización
  automática de todos los procesos línea y forzará la finalización de los procesos
  teléfono una vez hayan finalizados todos los procesos línea.
▪ Controlará el Apagado de emergencia de Obelix, ante la pulsación de Ctrl-C. Esto
  es:
        - Forzará la finalización de todos los procesos línea actualmente en
          funcionamiento.
        - Forzará la finalización de todos los procesos teléfono existentes.
▪ Por último, pero no menos importante, liberará todos los recursos utilizados.
  (semáforos, memoria compartida, etc.)
*/

void procesar_argumentos(int argc, char *argv[], int *numTelefonos, int *numLineas);   //*OK
void instalar_manejador_senhal();                                                      //*OK
void manejador_senhal(int sign);                                                       //*OK
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);            //*OK
void crear_procesos(int numTelefonos, int numLineas);                                  // TODO
void lanzar_proceso_telefono(const int indice_tabla);                                  // TODO
void lanzar_proceso_linea(const int indice_tabla);                                     // TODO
void esperar_procesos();                                                               // TODO
void terminar_procesos(void);                                                          // TODO
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num); // TODO
void liberar_recursos();                                                               // TODO

int g_telefonosProcesses = 0;
int g_lineasProcesses = 0;
struct TProcess_t *g_process_telefonos_table;
struct TProcess_t *g_process_lineas_table;

int main(int argc, char *argv[])
{
  // Define variables locales
  int numTelefonos;
  int numLineas;

  // Procesa los argumentos y los guarda en las dos variables
  procesar_argumentos(argc, argv, &numTelefonos, &numLineas);

  // Creamos semáforos y memoria compartida
  crear_sem(MUTEXESPERA, 1);
  crear_sem(TELEFONOS, 0);
  crear_sem(LINEAS, 0);
  crear_var(LLAMADASESPERA, 0); // No hay llamadas en espera

  // Manejador de Ctrl-C
  instalar_manejador_senhal();

  // Crea Tabla para almacenar los pids de los procesos
  iniciar_tabla_procesos(numTelefonos, numLineas);

  // Tenemos todo
  // Lanzamos los procesos
  crear_procesos(numTelefonos, numLineas);

  // Esperamos a que finalicen las lineas
  esperar_procesos();

  // Matamos los telefonos y cualquier otro proceso restante
  terminar_procesos();

  // Finalizamos Manager
  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
  liberar_recursos();

  return EXIT_SUCCESS;
}

// Si los argumentos no son el nombre el programa y el num. de lineas y telefonos, error.

void procesar_argumentos(int argc, char *argv[], int *nTelefonos, int *nLineas)
{

  if (argc != 3)
  {
    fprintf(stderr, "Error. Usa: ./exec/maager <nº teléfonos> <nº líneas>.\n");
    exit(EXIT_FAILURE);
  }
  //! Si por alguna razón los parámetros no se pillan bien, probar a deshacer esto con un simple atoi, he pillado esto de lo de ssoo 1
  if (sscanf(argv[1], "%d", &nTelefonos) != 1)
  {
    fprintf(stderr, "Error al obtener el número de teléfonos.\n");
    exit(EXIT_FAILURE);
  }

  if (sscanf(argv[2], "%d", &nLineas) != 1)
  {
    fprintf(stderr, "Error al obtener el número de líneas.\n");
    exit(EXIT_FAILURE);
  }
}

void instalar_manejador_senhal()
{
  if (signal(SIGINT, manejador_senhal) == SIG_ERR)
  {
    fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
void manejador_senhal(int sign)
{
  printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");
  terminar_procesos();
  liberar_recursos();
  exit(EXIT_SUCCESS);
}
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea)
{
  g_telefonosProcesses = n_procesos_telefono;
  g_lineasProcesses = n_procesos_linea;

  g_process_lineas_table = malloc(g_lineasProcesses * sizeof(struct TProcess_t));
  g_process_telefonos_table = malloc(g_telefonosProcesses * sizeof(struct TProcess_t));

  for (int i = 0; i < g_telefonosProcesses; i++)
  {
    g_process_telefonos_table[i].pid = 0;
  }

  for (int i = 0; i < g_lineasProcesses; i++)
  {
    g_process_lineas_table[i].pid = 0;
  }
}
void terminar_procesos()
{
  // Termina LINEAS, luego TELEFONOS. AMBOS ESPECIFICOS
  //! Importante, manager espera de manera natural a que acaben las líneas -> Forzamos apagado de teléfonos
  //! Pillar el bucle de puente de un solo carril
}
void liberar_recursos()
{
  // free de las tablas de procesos lineas y telefonos
  free(g_process_lineas_table);
  free(g_process_telefonos_table);
  //! En algún momento hay que cerrar y eliminar los semáforos: primitivas para cerrar (sem_close()) y eliminar (sem_unlink()) un semáforo, liberando así los recursos previamente creados por sem_open().
  //! De la misma manera, para la variable compartida. Ver práctica de PUENTE DE UN SOLO CARRIL.
}