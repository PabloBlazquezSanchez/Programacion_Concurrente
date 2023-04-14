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

// Si los argumentos no son el nombre el programa y el num. de lineas y telefonos, error.

void procesar_argumentos(int argc, char *argv[], int *nTelefonos, int *nLineas)
{

    if (argc != 3)
    {
        fprintf(stderr, "Error. Usa: ./exec/maager <nº teléfonos> <nº líneas>.\n");
        exit(EXIT_FAILURE);
    }
    //! Si por alguna razón los parámetros no se pillan bien, probar a deshacer esto con un simple atoi, he pillado esto de lo de ssoo 1
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

    printf("%d tel, %d lin.\n", *nTelefonos, *nLineas);
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
    // hfhf
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
    // Termina LINEAS, luego TELEFONOS. AMBOS ESPECIFICOS
    //! Importante, manager espera de manera natural a que acaben las líneas -> Forzamos apagado de teléfonos
    //! Pillar el bucle de puente de un solo carril

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
    // free de las tablas de procesos lineas y telefonos
    destruir_sem(LINEAS);
    destruir_sem(TELEFONOS);
    destruir_sem(MUTEXESPERA);
    destruir_var(LLAMADASESPERA);
    free(g_process_lineas_table);
    free(g_process_telefonos_table);
}