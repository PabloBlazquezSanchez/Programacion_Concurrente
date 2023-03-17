#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MAXBUFFER 4096

void comparar(char *buf, char *pat, int l);

int main(int argc, char *argv[])
{
  // TODO
  //!COMENTAR RAÚL
  // printf("\n<<%s>> , <<%s>>\n", argv[1], argv[2]);
  
  FILE *fdproces = fopen(argv[1], "r"); // abrir en solo lectura
  char *patron = argv[2];

  int no_linea = 1;
  char buffer[MAXBUFFER];
  fread(buffer, 1, MAXBUFFER, fdproces);

  comparar(buffer, patron, no_linea);

  fclose(fdproces);
  return EXIT_SUCCESS;
}

void comparar(char *buf, char *pat, int l)
{
  // fprintf(stdout, "Patron: %s\n", pat);
  // fprintf(stdout, "Buffer:\n%s\n", buf);
  char *linea = strtok(buf, "\n");
  char *aux = malloc(MAXBUFFER * sizeof(char));
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
}