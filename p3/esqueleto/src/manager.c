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

void crear_buzones();                                                                   //? Fijarse un poco en filosofos comensales T3
void instalar_manejador_senhal();                                                       //! Copia pega 2
void manejador_senhal(int sign);                                                        //! Copia pega 2
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);             //! Copia pega 2 o memset (mirar filosofos comensales T3)
void crear_procesos(int numTelefonos, int numLineas);                                   //! Copia pega 2
void lanzar_proceso_telefono(const int indice_tabla);                                   //? Fijarse en comensales T3 y tratar de hacer una mezcla
void lanzar_proceso_linea(const int indice_tabla);                                      //? Fijarse en comensales T3 y tratar de hacer una mezcla
void esperar_procesos();                                                                //! Copia pega 2, total, no va a hacer falta
void terminar_procesos(void);                                                           //! Copia pega 2
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);  //! Copia pega 2
void liberar_recursos();                                                                //? Fijarse en comensales T3 y tratar de hacer una mezcla

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

    // Matamos los telefonos y cualquier otro proceso restante
    terminar_procesos();

    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

// TODO
