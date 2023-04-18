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

void procesar_argumentos(int argc, char *argv[], int *numTelefonos, int *numLineas);
void instalar_manejador_senhal(); 
void manejador_senhal(int sign); 
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);
void crear_procesos(int numTelefonos, int numLineas);
void lanzar_proceso_telefono(const int indice_tabla);
void lanzar_proceso_linea(const int indice_tabla);
void esperar_procesos();
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);
void liberar_recursos();

int g_telefonosProcesses = 0;
int g_lineasProcesses = 0;
struct TProcess_t *g_process_telefonos_table; //Tabla de procesos telefono
struct TProcess_t *g_process_lineas_table; //Tabla de procesos linea

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
    // linea
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

/*
 * Este método se encarga de administrar los argumentos de la línea de órdenes,
 * ignorando el 0 (nombre del programa), el 1 es el número de téfonos y el 2 es
 * el número de líneas.
*/
void procesar_argumentos(int argc, char *argv[], int *nTelefonos, int *nLineas)
{

    if (argc != 3)
    {
        fprintf(stderr, "Error. Usa: ./exec/maager <nº teléfonos> <nº líneas>.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[1], "%d", nTelefonos) != 1)
    {
        fprintf(stderr, "Error al obtener el número de teléfonos.\n");
        exit(EXIT_FAILURE);
    }

    if (sscanf(argv[2], "%d", nLineas) != 1)
    {
        fprintf(stderr, "Error al obtener el número de líneas.\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Instala el manejador de la señal de interrupción, para que el
 * programa tenga un comportamiento determinado cuando llegue una.
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
 * Cuando llegue una señal de interrupción, el manejador hará por defecto
 * una terminación el programa, matando los procesos hijos creados,
 * liberando recursos y finalizando el principal, con éxito.
*/
void manejador_senhal(int sign)
{
    printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_SUCCESS);
}

/*
 * En este método se creará e inicializará dos tablas de procesos,
 * una para teléfonos y otra para líneas.
 *  -El total de procesos que tendrá la tabla de teléfonos será el número de teléfonos introducido en la linea de comandos.
 *  -El total de procesos que tendrá la tabla de lineas será el número de lineas introducido en la linea de comandos.
*/
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

/*
 * Desde este método se gestionará el arranque de los diferentes procesos.
 * Primero se inician los procesos telefono y luego se inician los procesos linea
 * ya que si lo hacemos al contrario tendriamos el inconveniente de que las lineas
 * que se van creando no encuentran ningún telefono libre.
*/
void crear_procesos(int numTelefonos, int numLineas)
{
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
        lanzar_proceso_linea(indice_tabla);
        indice_tabla++;
    }
    printf("[MANAGER] %d Lineas creadas.\n", indice_tabla);

    sleep(1);
}

/*
 * Para lanzar el proceso telefono requerimos el indice de tabla que posteriormente vamos a
 * usar para guardar el pid y la clase.
 * Creamos un subproceso con fork, el pid resultante (si no hay error) se guarda en la tabla de procesos
 * con el índice de tabla, pasado también a la función, junto con el tipo de proceso (CLASE).
*/
void lanzar_proceso_telefono(const int indice_tabla)
{
    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Error al lanzar proceso telefono: %s.\n", strerror(errno));
        terminar_procesos();
        liberar_recursos();
        exit(EXIT_FAILURE);
    case 0:
        if (execl(RUTA_TELEFONO, CLASE_TELEFONO, MUTEXESPERA, TELEFONOS, LINEAS, LLAMADASESPERA, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_TELEFONO, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    g_process_telefonos_table[indice_tabla].pid = pid;
    g_process_telefonos_table[indice_tabla].clase = CLASE_TELEFONO;
}

/*
 * Funciona de la misma manera que el método lanzar_proceso_telefono.
*/
void lanzar_proceso_linea(const int indice_tabla)
{
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Error al lanzar proceso telefono: %s.\n", strerror(errno));
        terminar_procesos();
        liberar_recursos();
        exit(EXIT_FAILURE);
    case 0:
        if (execl(RUTA_LINEA, CLASE_LINEA, MUTEXESPERA, TELEFONOS, LINEAS, LLAMADASESPERA, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_LINEA, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    g_process_lineas_table[indice_tabla].pid = pid;
    g_process_lineas_table[indice_tabla].clase = CLASE_LINEA;
}

/*
 * Para que manager (proceso principal) espere a procesos hijos, vamos a contar el número de
 * procesos restantes con n_processes. Mientras que queden procesos, hacemos un wait de cualquier hijo,
 * ya que cuando acabe uno de ellos, vamos a poner a cero su pid en la tabla de pids.
*/
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

/*
 * Este método presenta una función muy simple, va a llamar al método
 * terminar_procesos_especificos para finalizar por separado las lineas y los telefonos
 * en ese respectivo orden.
*/
void terminar_procesos()
{
    printf("\n----- [MANAGER] Terminar con cualquier proceso pendiente ejecutándose -----\n");
    terminar_procesos_especificos(g_process_lineas_table, g_lineasProcesses);
    terminar_procesos_especificos(g_process_telefonos_table, g_telefonosProcesses);
}

/*
 * Para finializar los procesos de una tabla debemos recorrer dicha tabla e ir comprobando si el pid de un proceso
 * es distinto de 0. En caso afirmativo procedemos a matar al proceso. Si al matarlo devuelve -1 imprime error al usar
 * la función kill().
*/
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

/*
 * Una vez se han terminado los procesos, este método va a destruir los semaforos
 * y la memoria compartida mediante las funciones destruir_sem y destruir_var de las
 * clases semaforo.c y memoria.c respectivamente, y va a liberar del espacio de 
 * memoria las tablas de procesos creadas al principio del programa mediante la función free().
*/
void liberar_recursos()
{
    destruir_sem(LINEAS);
    destruir_sem(TELEFONOS);
    destruir_sem(MUTEXESPERA);
    destruir_var(LLAMADASESPERA);
    free(g_process_lineas_table);
    free(g_process_telefonos_table);
}