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

void crear_buzones();
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
mqd_t qHandlerLlamadas;
mqd_t qHandlerLineas[NUMLINEAS];

int main(int argc, char *argv[])
{
    // Define variables locales

    // Creamos los buzones
    crear_buzones();

    // Manejador de Ctrl-C
    instalar_manejador_senhal();

    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos(NUMTELEFONOS, NUMLINEAS);

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos(NUMTELEFONOS, NUMLINEAS);

    // Esperamos a que finalicen las lineas
    esperar_procesos();

    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

void crear_buzones()
{
    int i;
    struct mq_attr mqAttr;
    mqAttr.mq_maxmsg = (NUMLINEAS);
    mqAttr.mq_msgsize = 15;
    char caux[24], buffer[64];

    // crear buzon llamadas de hasta 10 cajitas TODO CAMBIAR
    if ((qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttr)) == -1)
    {
        fprintf(stderr, "Error al crear el buzón de llamadas\n");
        liberar_recursos();
        exit(EXIT_FAILURE);
    }

    // pongo vacías las cajas:
    for (i = 0; i < NUMLINEAS; i++)
    {
        mq_send(qHandlerLlamadas, buffer, sizeof(buffer), 1);
    }

    // crear y poner vacíos los buzones lineas
    mqAttr.mq_maxmsg = 1;
    mqAttr.mq_msgsize = TAMANO_MENSAJES + 1;

    for (i = 0; i < NUMLINEAS; i++)
    {
        sprintf(caux, "%s%d", BUZON_LINEAS, i);
        if ((qHandlerLineas[i] = mq_open(caux, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttr)) == -1)
        {
            fprintf(stderr, "Error al crear el buzón de llamadas\n");
            liberar_recursos();
            exit(EXIT_FAILURE);
        }
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

    for (int i = 0; i < g_lineasProcesses; i++)
    {
        g_process_lineas_table[i].pid = 0;
    }

    for (int i = 0; i < g_telefonosProcesses; i++)
    {
        g_process_telefonos_table[i].pid = 0;
    }
}

void crear_procesos(int numTelefonos, int numLineas)
{
    int indice_tabla = 0;

    for (int i = 0; i < numLineas; i++)
    {
        lanzar_proceso_linea(indice_tabla);
        indice_tabla++;
    }
    printf("[MANAGER] %d Lineas creadas.\n", indice_tabla);

    sleep(2);
    indice_tabla = 0;

    for (int i = 0; i < numTelefonos; i++)
    {
        lanzar_proceso_telefono(indice_tabla);
        indice_tabla++;
    }
    printf("[MANAGER] %d Telefonos creados.\n", indice_tabla);
}

void lanzar_proceso_linea(const int indice_tabla)
{
    pid_t pid;
    char nombre_completo_buzon[24];
    sprintf(nombre_completo_buzon, "%s%d", BUZON_LINEAS, indice_tabla);

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Error al lanzar proceso telefono: %s.\n", strerror(errno));
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

    g_process_lineas_table[indice_tabla].pid = pid;
    g_process_lineas_table[indice_tabla].clase = CLASE_LINEA;
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
        if (execl(RUTA_TELEFONO, CLASE_TELEFONO, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_TELEFONO, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    g_process_telefonos_table[indice_tabla].pid = pid;
    g_process_telefonos_table[indice_tabla].clase = CLASE_TELEFONO;
}

void esperar_procesos()
{
    int i;
    for (i = 0; i < NUMLINEAS; i++)
        waitpid(g_process_lineas_table[i].pid, 0, 0);
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
            if (kill(process_table[i].pid, SIGKILL) == -1)
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

    free(g_process_lineas_table);
    free(g_process_telefonos_table);

    printf("\n[MANAGER] Liberando buzones... \n");
    printf("[MANAGER] Liberando Buzón %s\n", BUZON_LLAMADAS);
    mq_close(qHandlerLlamadas);
    mq_unlink(BUZON_LLAMADAS);

    for (i = 0; i < NUMLINEAS; i++)
    {
        sprintf(caux, "%s%d", BUZON_LINEAS, i);
        printf("[MANAGER] Liberando Buzón %s\n", caux);
        mq_close(qHandlerLineas[i]);
        mq_unlink(caux);
    }
}