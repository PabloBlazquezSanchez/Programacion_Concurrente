#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

void comparar(char *buf, char *pat, int l);

/*
Se pasarán de manager la ruta del archivo (argumento 1) y el patrón (argumento 2). Se irá leyendo el contenido
del archivo al que apunta su descriptor, y se guardará en un buffer. Este último será el que utilizaremos para
desglosar en líneas para buscar coincidencias con el patrón. Finalmente se cierra el archivo.
*/

int main(int argc, char *argv[])
{
  FILE *fdproces = fopen(argv[1], "r");
  char *patron = argv[2];
  int no_linea = 1;
  char buffer[PATH_MAX];
  fread(buffer, sizeof(char), PATH_MAX, fdproces);
  comparar(buffer, patron, no_linea);
  fclose(fdproces);
  return EXIT_SUCCESS;
}

/*
Para poder comparar el patrón con cada línea, iremos partiendo el buffer con strtok por líneas (resultado en
puntero linea). También declaramos otro puntero de caracteres auxiliar, donde le escribiremos la salida de hacer
un strstr con la línea y el patrón. Podremos así comparar y partir el contenido del fichero cíclicamente.
Este comando devuelve un puntero de caracteres, que apunta a la primera coincidencia de 'pat' (patrón) en 'linea'.
Por lo tanto si su contenido no es un carácter nulo, es una coincidencia.
*/

void comparar(char *buf, char *pat, int l)
{
  char *linea = strtok(buf, "\n");
  char *aux = malloc(PATH_MAX * sizeof(char));
  while (linea != NULL)
  {
    sprintf(aux, strstr(linea, pat));
    if (*aux != '\0')
    {
      fprintf(stdout, "[PROCESADOR %d] Patron '%s' encontrado en linea %d\n", getpid(), pat, l);
    }
    linea = strtok(NULL, "\n");
    l++;
  }
  free(aux);
}