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
#include <lista.h>

void procesar_argumentos(int argc, char *argv[], char **filename, char **pattern, int *lines);
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void procesar_patrones(const char *fichero_patrones);
void procesar_linea(char *linea);
void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador);
void crear_procesos(const char *nombre_fichero);
void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str);
void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero);
void esperar_procesos();
void terminar_procesos(void);
void liberar_recursos();

int g_nProcesses;
struct TProcess_t *g_process_table;
TLista *patrones;

/*
El orden que seguirá el manager para su ejecución completa será crear una lista de patrones,
procesar argumentos y patrones, instalar el manejador de la señal de interrupción, crear tabla
de procesos para poder lanzarlos, esperar a su fin de ejecución y liberación de recursos
*/

int main(int argc, char *argv[])
{
  char *nombre_fichero = NULL, *fichero_patrones = NULL;
  int lineas = 0;
  patrones = malloc(sizeof(TLista));

  crear(patrones, "PATRONES"); //CREA UNA LISTA DE PATRONES CUYO PRIMER VALOR ES PATRONES
  procesar_argumentos(argc, argv, &nombre_fichero, &fichero_patrones, &lineas);

  instalar_manejador_senhal();
  procesar_patrones(fichero_patrones);
  iniciar_tabla_procesos(lineas, longitud(patrones) - 1);
  crear_procesos(nombre_fichero);
  esperar_procesos();

  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
  liberar_recursos();

  return EXIT_SUCCESS;
}

/*
Este método se encarga de administrar los argumentos de la línea de órdenes, ignorando el 0 (nombre del programa), el 1
es el archivo solución y el 2 es el fichero de patrones. También lee el fichero solución y cuenta las líneas que tiene.
*/

void procesar_argumentos(int argc, char *argv[], char **nombrefichero, char **fichero_patrones, int *lineas)
{
  FILE *fp;
  int ch;

  if (argc != 3)
  {
    fprintf(stderr, "Error. Usa: ./exec/manager <fichero> <fichero_patrones>.\n");
    exit(EXIT_FAILURE);
  }

  *nombrefichero = argv[1];
  *fichero_patrones = argv[2];

  if ((fp = fopen(*nombrefichero, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", *nombrefichero);
    exit(EXIT_FAILURE);
  }

  while ((ch = fgetc(fp)) != EOF)
  {
    if (ch == '\n')
    {
      ++*lineas;
    }
  }

  fclose(fp);
}

/*
Instala el manejador de la señal de interrupción, para que el programa tenga un comportamiento determinado
cuando llegue una.
*/

void instalar_manejador_senhal()
{
  if (signal(SIGINT, manejador_senhal) == SIG_ERR)
  {
    fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/*
Cuando llegue una señal de interrupción, el manejador hará por defecto una terminación el programa, matando los procesos hijos
creados, liberando recursos y finalizando el principal, con éxito.
*/

void manejador_senhal(int sign)
{
  printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");
  terminar_procesos();
  liberar_recursos();
  exit(EXIT_SUCCESS);
}

/*
El siguiente método recoge todos los patrones del fichero de patrones en un array de caracteres
(long. máxima = PATH_MAX). Para ello se obtienen las líneas del fichero de patrones, para poder
procesarlas (más adelante). Cuando acaba, cierra el fichero.
*/

void procesar_patrones(const char *fichero_patrones)
{
  FILE *fp;
  char linea[PATH_MAX];

  if ((fp = fopen(fichero_patrones, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", fichero_patrones);
    exit(EXIT_FAILURE);
  }

  while (fgets(linea, sizeof(linea), fp) != NULL)
  {
    procesar_linea(linea);
  }

  fclose(fp);
}

/*
Para procesar una línea, se divide el array 'linea', en función de los espacios, y por cada token obtenido,
que será un patrón, se inserta a la lista de patrones, hasta que no quede nada en el array.
*/

void procesar_linea(char *linea)
{
  char *token;

  token = strtok(linea, " ");
  while (token != NULL)
  {
    insertarFinal(patrones, token);
    token = strtok(NULL, " ");
  }
}

/*
En este método se creará e inicializará la tabla de procesos.
El total de procesos que tendrá la tabla será el número de líneas
a procesar (3) y la longitud de la lista de patrones, menos uno (3) = 6.
*/

void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador)
{
  g_nProcesses = n_procesos_contador + n_procesos_procesador;
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t));

  for (int i = 0; i < g_nProcesses; i++)
  {
    g_process_table[i].pid = 0;
  }
}

/*
Desde este método se gestionará el arranque de los diferentes procesos. Los procesos 0, 1, 2 (índices
de tabla que se escriben en numero_linea_str) serán contadores, 3, 4, 5 procesadores. Al acabar cierra
el archivo, tras paralizarse 1 segundo el manager.
*/

void crear_procesos(const char *nombre_fichero)
{
  FILE *fp;
  char linea[PATH_MAX], numero_linea_str[/*3*/11]; //! Cambiado de 3 a 11 para evitar un warning de desbordamiento de array (recomendación GCC ver.12.2.0-14)
  int indice_tabla = 0;
  
  if ((fp = fopen(nombre_fichero, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", nombre_fichero);
    exit(EXIT_FAILURE);
  }

  while (fgets(linea, sizeof(linea), fp) != NULL)
  {
    sprintf(numero_linea_str, "%d", indice_tabla);
    lanzar_proceso_contador(indice_tabla, linea, numero_linea_str);
    indice_tabla++;
  }

  for (int i = 1; i < longitud(patrones); i++) //! Antes empezaba desde 2 hasta longitud de patrones, incluida. Nuestra lista empieza en 0 -> quitamos 1 iteración
  {
    lanzar_proceso_procesador(indice_tabla, getElementoN(patrones, i), nombre_fichero);
    indice_tabla++;
  }

  printf("[MANAGER] %d procesos creados.\n", indice_tabla);
  sleep(1);

  fclose(fp);
}

/*
Para poder lanzar un proceso contador, requerimos la línea para contar las palabras y el número de línea:
  - Le pasamos la línea sacada anteriormente con fgets().
  - Y el numero_linea_str guarda la línea a contar, de hecho como es el mismo número que el índice de tabla,
    escribimos con sprintf éste en numero_linea_str.
Creamos un subproceso con fork, el pid resultante (si no hay error) se guarda en la tabla de procesos
con el índice de tabla, pasado también a la función, junto con el tipo de proceso (CLASE).
*/

void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str)
{
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso contador: %s.\n", strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_CONTADOR, CLASE_CONTADOR, linea, numero_linea_str, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el poceso %s: %s.\n", CLASE_CONTADOR, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_table[indice_tabla].pid = pid;
  g_process_table[indice_tabla].clase = CLASE_CONTADOR;
}

/*
Similar al proceso anterior, solo que en lugar de pasar el número de línea y la línea le pasamos:
  - Nombre del fichero solución
  - Patrón para buscar coincidencias
*/

void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero)
{
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso procesador: %s.\n", strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_PROCESADOR, CLASE_PROCESADOR, nombre_fichero, patron, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PROCESADOR, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_table[indice_tabla].pid = pid;
  g_process_table[indice_tabla].clase = CLASE_PROCESADOR;
}

/*
Para que manager (proceso principal) espere a procesos hijos, vamos a contar el número de
procesos restantes con n_processes. Mientras que queden procesos, hacemos un wait de cualquier hijo,
ya que cuando acabe uno de ellos, vamos a poner a cero su pid en la tabla de pids.
*/

void esperar_procesos()
{
  int i, n_processes = g_nProcesses;
  pid_t pid;

  while (n_processes > 0)
  {
    pid = wait(NULL);
    for (i = 0; i < g_nProcesses; i++)
    {
      if (pid == g_process_table[i].pid)
      {
        printf("[MANAGER] Proceso %s terminado [%d]...\n", g_process_table[i].clase, g_process_table[i].pid);
        g_process_table[i].pid = 0;
        n_processes--;
        break;
      }
    }
  }
}

/*
Cuando acabe el proceso padre (por SUCESS o FAILURE), se procede a liberar el espacio de
memoria donde está la tabla de procesos y a destruir la lista de patrones.
*/

void liberar_recursos()
{
  free(g_process_table);
  destruir(patrones);
}

/*
Cuando la ejecución acabe con error o interrupción, se procede a poner a 0 todos los pid de
la tabla de procesos creados por el manager, y enviar una señal de interrupción a estos,
matando así su ejecución (usando kill())
*/

void terminar_procesos(void)
{
  int i;

  printf("\n----- [MANAGER] Terminar con los procesos hijos ejecutándose ----- \n");
  for (i = 0; i < g_nProcesses; i++)
  {
    if (g_process_table[i].pid != 0)
    {
      printf("[MANAGER] Terminando proceso %s [%d]...\n", g_process_table[i].clase, g_process_table[i].pid);
      if (kill(g_process_table[i].pid, SIGINT) == -1)
      {
        fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", g_process_table[i].pid, strerror(errno));
      }
    }
  }
}