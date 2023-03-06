#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

int tamaño = 0;

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
  pLista->pUltimo = pLista->pPrimero;
  tamaño = 1;
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
  tamaño = 0;
}

// Inserta al principio/inicio de la lista.
/*
TODO COMENTARIO TO GUAPO AHI A LO GUAY
*/
void insertar(TLista *pLista, int valor)
{
  TNodo *nuevo = malloc(sizeof(TNodo));
  nuevo->valor = valor;
  nuevo->pSiguiente = pLista->pPrimero;
  pLista->pPrimero = nuevo;
  tamaño++;
}

// Inserta al final de la lista.
// TODO COMENTARIO TO GUAPO AHI A LO GUAY

void insertarFinal(TLista *pLista, int valor)
{
  TNodo *nuevo = malloc(sizeof(TNodo));
  nuevo->valor = valor;
  pLista->pUltimo->pSiguiente = nuevo;
  pLista->pUltimo = nuevo;
  pLista->pUltimo->pSiguiente = NULL;
  tamaño++;
}

// Suponemos n = 1, 2, ...

// TODO COMENTARIO TO GUAPO AHI A LO GUAY

void insertarN(TLista *pLista, int index, int valor)
{

  if (index < 0 || index > tamaño - 1)
  {
    printf("Posición ilegal. Como el Aston Martin de Fernando\n");
    exit(-1);
  }
  else
  {
    TNodo *aux = malloc(sizeof(TNodo));
    if (index == 0)
    {
      aux->valor = valor;
      aux->pSiguiente = pLista->pPrimero;
      pLista->pPrimero = aux;
    }
    else
    {
      TNodo *nodoiterativo = malloc(sizeof(TNodo));
      TNodo *aux2 = malloc(sizeof(TNodo));
      nodoiterativo = pLista->pPrimero;

      for (int i = 1; i <= index; i++)
      {
        nodoiterativo = nodoiterativo->pSiguiente;
      }
      aux2->valor = nodoiterativo->valor;
      aux2->pSiguiente = nodoiterativo->pSiguiente;

      nodoiterativo->pSiguiente = aux2;
      nodoiterativo->valor = valor;
    }
  }
  tamaño++;
}

//  Elimina el primer elemento de la lista.
//! AHORA MISMO ELIMINA EL ULTIMO.
// TODO COMENTARIO TO GUAPO AHI A LO GUAY
void eliminar(TLista *pLista)
{
  /*BORRA EL PRIMERO*/

  // TNodo *elim = pLista->pPrimero;
  // pLista->pPrimero = pLista->pPrimero->pSiguiente;
  // free(elim);
  // tamaño--;

  /*BORRA EL ÚLTIMO*/

  if (tamaño == 1)
  {
    //free(pLista->pPrimero);
    //pLista->pPrimero = NULL;
    //pLista->pUltimo = NULL;
    destruir(pLista);
  }
  else
  {
    TNodo *pElim;
    for (pElim = pLista->pPrimero; pElim->pSiguiente->pSiguiente != NULL; pElim = pElim->pSiguiente)
      ;
    pLista->pUltimo = pElim;
    free(pElim->pSiguiente);
    pElim->pSiguiente = NULL;
    tamaño--;
  }
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
void eliminarN(TLista *pLista, int index)
{
  if (index < 0 || index > tamaño - 1)
  {
    printf("Posición ilegal. Como el Red Bull de Verstappen\n");
    exit(-1);
  }
  else
  {
    TNodo *aux = pLista->pPrimero;
    if (index == 0)
    {
      pLista->pPrimero = pLista->pPrimero->pSiguiente;
      free(aux);
    }
    else
    {
      TNodo *nodoiterativo = pLista->pPrimero;
      TNodo *borrar = NULL;
      for (int i = 1; i < index; i++)
      {
        if (nodoiterativo->pSiguiente == NULL)
        {
          printf("Roto\n");
          exit(-1);
        }
        nodoiterativo = nodoiterativo->pSiguiente;
      }
      borrar = nodoiterativo->pSiguiente;
      nodoiterativo->pSiguiente = borrar->pSiguiente;
      if (borrar->pSiguiente == NULL)
      { // este es el ultimo elemento
        pLista->pUltimo = nodoiterativo;
      }
      free(borrar);
    }
  }
  tamaño--;
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
int getElementoN(TLista *pLista, int index)
{
  if (index < 0 || index > tamaño - 1)
  {
    printf("Posición ilegal. Como el Mercedes de Hamilton\n");
    exit(-1);
  }
  TNodo *aux = pLista->pPrimero;
  for (int i = 0; i < index; i++)
    aux = aux->pSiguiente;
  return aux->valor;
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
void imprimir(TLista *pLista)
{
  for (TNodo *pIterador = pLista->pPrimero; pIterador != NULL; pIterador = pIterador->pSiguiente)
    printf("[ %d ]", pIterador->valor);
  printf("\n");
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
int longitud(TLista *pLista)
{
  return tamaño;
}