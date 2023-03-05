#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

// Crea una lista con un nodo.

/*
* Para crear la lista, le pasamos el puntero con el nombre de la lista (pLista) y un valor entero
* como contenido. En principio, se crea una lista con un único nodo (y su entero), que será EL PRIMERO.
* Por eso su asignación dinámica en memoria es coon el tamaño del nodo 'TNodo'.
* Aqui guardaremos el valor entero y un siguiente nodo apuntando a NULL (hasta que se agregue otro elemento).
*/

void crear(TLista *pLista, int valor)
{
  pLista->pPrimero = malloc(sizeof(TNodo));
  pLista->pPrimero->valor = valor;
  pLista->pPrimero->pSiguiente = NULL;
}

/*
* En cambio, para destruir la lista, lo que se hace de manera iterativa es limpiar los espacios de memoria de cada
* nodo. Para ello, se declaran dos punteros de estructura nodo. El primero empezará apuntando al primer elemento de
* la lista. Para cada iteración, el segundo puntero apuntará al siguiente elemento. Así, se puede liberar el espacio
* de memoria al que apunta el primer puntero, para después asignar la misma dirección de memoria a la que apunta el segundo.
* El proceso termina cuando el primer puntero apunta a NULL, entonces, se libera el espacio de memoria de la lista.
*/

void destruir(TLista *pLista)
{
  TNodo *pAux1;
  struct Nodo *pAux2;

  for (pAux1 = pLista->pPrimero; pAux1 != NULL;)
  {
    pAux2 = pAux1->pSiguiente;
    free(pAux1);
    pAux1 = pAux2;
  }

  free(pLista);
}

// Inserta al principio/inicio de la lista.
void insertar(TLista *pLista, int valor)
{
  // TODO
}

// Inserta al final de la lista.
void insertarFinal(TLista *pLista, int valor)
{
  // TODO
}

// Suponemos n = 1, 2, ...
void insertarN(TLista *pLista, int index, int valor)
{
  // TODO
}

// Elimina el primer elemento de la lista.
void eliminar(TLista *pLista)
{
  // TODO
}

void eliminarN(TLista *pLista, int index)
{
  // TODO
}

int getElementoN(TLista *pLista, int index)
{
  // TODO
  return 0;
}

void imprimir(TLista *pLista)
{
  // TODO
  /*
  mientras que nodo (que será el primero) no sea nulo, imprimir nodo->valor y nodo=nodo siguiente (avanzar)
  */
}

int longitud(TLista *pLista)
{
  // TODO
}