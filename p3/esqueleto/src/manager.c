#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <mqueue.h>
#include <definitions.h>

void crear_buzones();                                                                  //? Fijarse un poco en filosofos comensales T3
void instalar_manejador_senhal();                                                      //* Copia pega 2
void manejador_senhal(int sign);                                                       //* Copia pega 2
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);            //* Copia pega 2 o memset (mirar filosofos comensales T3)
void crear_procesos(int numTelefonos, int numLineas);                                  //! Copia pega 2
void lanzar_proceso_telefono(const int indice_tabla);                                  //? Fijarse en comensales T3 y tratar de hacer una mezcla
void lanzar_proceso_linea(const int indice_tabla, const char *nombre_completo_buzon);  //? Fijarse en comensales T3 y tratar de hacer una mezcla
void esperar_procesos();                                                               //! No va a hacer falta
void terminar_procesos(void);                                                          //* Copia pega 2
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num); //* Copia pega 2
void liberar_recursos();                                                               //? Fijarse en comensales T3 y tratar de hacer una mezcla

int g_telefonosProcesses = 0;
int g_lineasProcesses = 0;
struct TProcess_t *g_process_telefonos_table;
struct TProcess_t *g_process_lineas_table;
mqd_t qHandlerLlamadas;
mqd_t qHandlerLineas[NUMLINEAS];
struct mq_attr mqAttr;
/*
▪ Inicializar las colas de mensajes necesarias para establecer la comunicación entre
  líneas y teléfonos, así como, la comunicación entre los teléfonos y cada una de las
  líneas.

▪ Lanzará el número de procesos teléfono y procesos línea definidos en este caso como
  constantes del sistema.
  ! Este punto es copia pega del proy anterior

▪ El sistema, gracias al paso de mensajes, no tendrá finalización, por lo que el
  Apagado Controlado de Obelix se realizará mediante la pulsación de Ctrl-C.
  ! Esperar_procesos() tal vez no sea necesario => KILL a todo

• Forzará la finalización de todos los procesos línea actualmente en
  funcionamiento.
  ! Vuelta alo mismo del apartado anterior

• Forzará la finalización de todos los procesos teléfono existentes.
  ! Este punto es copia pega del proy anterior

▪ Por último, pero no menos importante, liberará todos los recursos utilizados. (tablas
  de procesos, colas de mensajes, etc.)
*/

int main(int argc, char *argv[])
{
  // Define variables locales

  // Creamos los buzones
  //* El sistema operativo tiene un máximo de 10 mensajes para las colas de mensajes, por lo que el número de líneas de Obelix será de 10. #DEFINE + control?

  /*
  Ojo con las colas. Son independientes de los procesos y, por tanto, si la cola se queda creada
  y con mensajes, en la siguiente ejecución de los procesos nos pueden aparecer resultados no
  controlados.
  */

  crear_buzones();

  // Manejador de Ctrl-C
  instalar_manejador_senhal();

  // Crea Tabla para almacenar los pids de los procesos
  iniciar_tabla_procesos(NUMTELEFONOS, NUMLINEAS);

  // Tenemos todo
  // Lanzamos los procesos

  //* Crear antes los procesos de línea que los de teléfono.
  //! Si entra una llamada se quedará almacenada en la cola de “Llamadas a ser atendidas” hasta que estén disponibles los teléfonos.

  crear_procesos(NUMTELEFONOS, NUMLINEAS);

  // Esperamos a que finalicen las lineas
  esperar_procesos(); //! QUE NO HACE FALTA COÑO YA

  // Matamos los telefonos y cualquier otro proceso restante
  terminar_procesos();

  // Finalizamos Manager
  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
  liberar_recursos();

  return EXIT_SUCCESS;
}

// TODO
void crear_buzones()
{
  // TODO
  mqAttr.mq_maxmsg = (NUMLINEAS);
  mqAttr.mq_msgsize = TAMANO_MENSAJES;
  char caux[30], buffer[64];

  // crear buzon llamadas de hasta 10 cajitas TODO CAMBIAR
  qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttr);

  // pongo vacías las cajas:
  for (int i = 0; i < NUMLINEAS; i++)
  {
    mq_send(qHandlerLlamadas, buffer, sizeof(buffer), 1);
  }

  // crear y poner vacíos los buzones lineas
  mqAttr.mq_maxmsg = 1;
  for (int i = 0; i < NUMLINEAS; i++)
  {
    sprintf(caux, "%s%d", BUZON_LINEAS, i);
    qHandlerLineas[i] = mq_open(caux, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttr);
    mq_send(qHandlerLineas[i], buffer, sizeof(buffer), 0);
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

void crear_procesos(int numTelefonos, int numLineas)
{
  char nombre_completo_buzon[30];
  int indice_tabla = 0;
  for (int i = 0; i < numTelefonos; i++)
  {
    lanzar_proceso_telefono(indice_tabla);
    indice_tabla++;
  }
  printf("[MANAGER] %d Telefonos creados.\n", indice_tabla);

  indice_tabla = 0;

  for (int i = 0; i < numLineas; i++)
  {
    sprintf(nombre_completo_buzon, " %s %d", BUZON_LINEAS, indice_tabla);
    lanzar_proceso_linea(indice_tabla, nombre_completo_buzon);
    indice_tabla++;
  }
  printf("[MANAGER] %d Lineas creadas.\n", indice_tabla);

  sleep(1);
}

void lanzar_proceso_linea(const int indice_tabla, const char *nombre_completo_buzon)
{
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso %s: %s.\n", CLASE_LINEA, strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_LINEA, CLASE_LINEA, nombre_completo_buzon, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_LINEA, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_telefonos_table[indice_tabla].pid = pid;
  g_process_telefonos_table[indice_tabla].clase = CLASE_LINEA;
}

void lanzar_proceso_telefono(const int indice_tabla)
{
  // execl(RUTA_TELEFONO, CLASE_TELEFONO, BUZON_LLAMADAS, NULL);
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso %s: %s.\n", CLASE_TELEFONO, strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_TELEFONO, CLASE_TELEFONO, BUZON_LLAMADAS, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_TELEFONO, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_telefonos_table[indice_tabla].pid = pid;
  g_process_telefonos_table[indice_tabla].clase = CLASE_TELEFONO;
}

//!! ESTE MÉTODO NO ES NECESARIO, PUESTO QUE NO ESPERO A QUE TERMINEN LOS PROCESOS, PUES NUNCA TERMINAN.
void esperar_procesos()
{
  int i, n_processes = g_lineasProcesses;
  pid_t pid;

  while (n_processes > 0)
  {
    pid = wait(NULL);
    for (i = 0; i < g_lineasProcesses; i++)
    {
      if (pid == g_process_lineas_table[i].pid)
      {
        printf("[MANAGER] Proceso LINEA terminado [%d]...\n", g_process_lineas_table[i].pid);
        g_process_lineas_table[i].pid = 0;
        n_processes--;
        break;
      }
    }
  }
}

void terminar_procesos()
{
  printf("\n----- [MANAGER] Terminar con cualquier proceso pendiente ejecutándose -----\n");
  terminar_procesos_especificos(g_process_lineas_table, g_lineasProcesses);
  terminar_procesos_especificos(g_process_telefonos_table, g_telefonosProcesses);
}

void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num)
{
  for (int i = 0; i < process_num; i++)
  {
    if (process_table[i].pid != 0)
    {
      printf("[MANAGER] Terminando proceso %s [%d]...\n", process_table[i].clase, process_table[i].pid);
      if (kill(process_table[i].pid, SIGINT) == -1)
      {
        fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno));
      }
    }
  }
}

void liberar_recursos()
{
  int i;
  char caux[60];
  for (i = 0; i < NUMTELEFONOS; i++)
  {
    sprintf(caux, " %s %d", BUZON_LLAMADAS, i);
    mq_close(qHandlerLlamadas);
    mq_unlink(BUZON_LLAMADAS);
  }

  for (i = 0; i < NUMLINEAS; i++)
  {
    sprintf(caux, " %s %d", BUZON_LINEAS, i);
    mq_close(qHandlerLineas[i]);
    mq_unlink(caux);
  }
}