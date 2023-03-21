#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void contar(char *line, int line_number);

//!COMENTAR PABLO

//! execl(RUTA_CONTADOR, CLASE_CONTADOR, linea, numero_linea_str, NULL)
//!   argv      X             0           1           2           NULL
// Se pasan de manager la línea (argumento 1) y el número de línea en string (argumento 2) y se ejecuta el método contar
int main(int argc, char *argv[])
{
  contar(argv[1], atoi(argv[2])); //atoi == cast() de java
  
  return EXIT_SUCCESS;
}

/*
 * El método contar se va a encargar de darnos a conocer cuántas palabras tiene una línea. Para ello se definen tres variables,
 * una recoge en número de palabras, otra que comprueba si estamos dentro de una palabra y un iterador que toma el valor de la línea.
 * Mediante un bucle do-while se irá recorriendo la línea y visualizando si hay valores nulos(\0), espacios(" "), tabulaciones(\t), saltos de línea(\n)
 * y recorrimientos de cursor a la izquierda(\n), donde comprobará si es cualquier otra cosa. En caso afirmativo, se tratará de una palabra
 * y se sumará una unidad a la variable n_palabras y se cambia el valor del comprobante a 0. Se realiza de forma iterativa hasta que
 * se termina de recorrer la línea. Entonces se imprime el número de palabras de esa línea.
*/
void contar(char *linea, int numero_linea)
{
  int n_palabras = 0, dentro_palabra = 0;
  const char *it = linea;

  do
  {
    switch (*it)
    {
    case '\0':
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      if (dentro_palabra)
      {
        dentro_palabra = 0;
        n_palabras++;
      }
      break;
    default:
      dentro_palabra = 1;
    }
  } while (*it++);

  printf("[CONTADOR %d] La linea '%d' tiene %d palabras\n", getpid(), numero_linea, n_palabras);
}