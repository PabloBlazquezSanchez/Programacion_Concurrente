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
    struct mq_attr mqAttrA, mqAttrB;
    mqAttrA.mq_maxmsg = NUMLINEAS;
    mqAttrA.mq_msgsize = TAMANO_MENSAJES;
    char caux[TAMANO_MENSAJES];

    // crear buzon llamadas de hasta 10 cajitas TODO CAMBIAR
    if ((qHandlerLlamadas = mq_open(BUZON_LLAMADAS, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttrA)) == -1)
    {
        fprintf(stderr, "Error al crear el buzón de llamadas\n");
        liberar_recursos();
        exit(EXIT_FAILURE);
    }

    // pongo vacías las cajas:
    // for (i = 0; i < NUMLINEAS; i++)
    // {
    //     mq_send(qHandlerLlamadas, buffer, sizeof(buffer), 1);
    // }

    // crear y poner vacíos los buzones lineas
    mqAttrB.mq_maxmsg = 1;
    mqAttrB.mq_msgsize = TAMANO_MENSAJES;

    for (i = 0; i < NUMLINEAS; i++)
    {
        sprintf(caux, "%s%d", BUZON_LINEAS, i);
        if ((qHandlerLineas[i] = mq_open(caux, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &mqAttrB)) == -1)
        {
            fprintf(stderr, "Error al crear el buzón de llamadas\n");
            liberar_recursos();
            exit(EXIT_FAILURE);
        }
        // mq_send(qHandlerLineas[i], buffer, sizeof(buffer), 0); //?
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

    for (int i = 0; i < g_lineasProcesses; i++)
    {
        g_process_lineas_table[i].pid = 0;
    }

    for (int i = 0; i < g_telefonosProcesses; i++)
    {
        g_process_telefonos_table[i].pid = 0;
    }
}

/*
 * Desde este método se gestionará el arranque de los diferentes procesos.
 * Primero se inician los procesos linea y luego se inician los procesos telefono.
 */
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

/*
 * Para lanzar el proceso linea requerimos del inidice de tabla que posteriormente vamos a usar para guardar 
 * el pid y la clase.
 * Creamos un subproceso con fork, el pid resultante (si no hay error) se guarda en la tabla de procesos con 
 * el indice de tabla, pasado también a la función, junto con el tipo de proceso (CLASE).
*/
void lanzar_proceso_linea(const int indice_tabla)
{
    pid_t pid;
    char nombre_completo_buzon[TAMANO_MENSAJES];
    sprintf(nombre_completo_buzon, "%s%d", BUZON_LINEAS, indice_tabla);

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Error al lanzar proceso linea: %s.\n", strerror(errno));
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

/*
 * Funciona de la misma manera que el método lanzar_proceso_linea.
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
        if (execl(RUTA_TELEFONO, CLASE_TELEFONO, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_TELEFONO, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    g_process_telefonos_table[indice_tabla].pid = pid;
    g_process_telefonos_table[indice_tabla].clase = CLASE_TELEFONO;
}

/*
 * Para que manager (proceso principal) espere a procesos hijos, vamos a contar el número de
 * lineas restantes con NUMLINEAS. Mientras que queden procesos, hacemos un wait de cualquier linea.
 */
void esperar_procesos()
{
    int i;
    for (i = 0; i < NUMLINEAS; i++)
        waitpid(g_process_lineas_table[i].pid, 0, 0);
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
            if (kill(process_table[i].pid, SIGKILL) == -1)
            {
                fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno));
            }
        }
    }
}

/*
 * Una vez se han terminado los procesos, primero vamos a liberar las tablas de procesos linea y teledono.
 * Tras esto, vamos a cerrar y desvincular el buzón de llamadas y luego vamos a hacer lo mismo con los buzones
 * linea.
 */
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