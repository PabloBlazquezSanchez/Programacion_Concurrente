#include <stdio.h>
#include <stdlib.h>
#include <lista.h>
#include <string.h>

// Crea una lista con un nodo.
//!RESUMIR Y TERMINAR DE COMENTAR PABLO

/*
 * Para crear la lista, le pasamos el puntero con el nombre de la lista (pLista) y un valor entero
 * como contenido. En principio, se crea una lista con un único nodo (y su entero), que será EL PRIMERO y EL ÚLTIMO
 * (hasta que se introduzcan nodos nuevos o se destruya la lista).
 * Por eso su asignación dinámica en memoria es coon el tamaño del nodo 'TNodo'.
 * Aqui guardaremos el valor entero y un siguiente nodo apuntando a NULL (hasta que se agregue otro elemento).
 */

void crear(TLista *pLista, char valor[])
{
  pLista->pPrimero = malloc(sizeof(TNodo));
  // pLista->pPrimero->valor = valor;
  pLista->pPrimero->valor = malloc(strlen(valor)*sizeof(char));
  strcpy(pLista->pPrimero->valor, valor);
  pLista->pPrimero->pSiguiente = NULL;
  pLista->pUltimo = pLista->pPrimero;
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
/*
 * Para insertar un elemento a la pila le vamos a pasar el puntero con el nombre de la pila y
 * un entero que contiene el valor a insertar. Se crea un puntero de tipo TNodo al que se le
 * va a asignar un espacio de memoria equivalente al tamaño de TNodo. Una vez creado le vamos a
 * dar el valor introducido y le vamos a asignar como siguiente elemento, es decir, el elemento
 * de la lista que va despúes de él, el primer elemento de la lista
 * pLista->pPrimero ES EL PRIMER ELEMENTO DE LA LISTA 'pLista'. EQUIVALENTE A LISTA ADT = HEAD O CABEZA
 * pLista->pUltimo ES EL ULTIMO ELEMENTO DE LA LISTA 'pLista'. EQUIVALENTE A LISTA ADT = TAIL O COLA
 */
void insertar(TLista *pLista, char valor[])
{
  TNodo *nuevo = malloc(sizeof(TNodo));
  // nuevo->valor = valor;
  nuevo->valor = malloc(strlen(valor)*sizeof(char));
  strcpy(nuevo->valor, valor);
  nuevo->pSiguiente = pLista->pPrimero;
  pLista->pPrimero = nuevo;
}

// Inserta al final de la lista.
// TODO COMENTARIO TO GUAPO AHI A LO GUAY --PABLO

void insertarFinal(TLista *pLista, char valor[])
{
  TNodo *nuevo = malloc(sizeof(TNodo));
  // nuevo->valor = valor;
  pLista->pUltimo->pSiguiente = nuevo;
  pLista->pUltimo = nuevo;
  pLista->pUltimo->valor = malloc(strlen(valor)*sizeof(char));
  strcpy(pLista->pUltimo->valor, valor);
  pLista->pUltimo->pSiguiente = NULL;
}

// Suponemos n = 1, 2, ...

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
/*
 * Para insertar un valor cualquiera en una posición N de la lista, presuponemos que se inserta detrás de dicha posición N.
 * Lo primero que haremos será controlar el valor de índice que se le pasa a la función, siendo incorrectos los valores nulos
 * o aquellos que sobrepasen el valor del último índice de la lista (= tamaño - 1).
 *
 * Una vez tenemos un índice válido, declaramos un nodo de inserción al que le hacemos una asignación en memoria con el
 * tamaño de un nodo. Despúes, discriminamos si el índice es 0 o >=1:
 *     - Para index = 0 procedemos similar al método insertar()
 *     - Para index >= 1, nos vamos a apoyar en otro nodo auxiliar que hemos denominado como iterativo, ya que irá avanzando las
 *       posiciones de la lista ("nodo iterativo = su siguiente nodo"). Una vez el iterador se ha colocado en la posición 'index',
 *       clonamos la información del nodo iterativo en el de inserción, ya que el nodo iterativo con posición 'n' es equivalente
 *       a tener el elemento 'n' de la lista enlazada.
 */

void insertarN(TLista *pLista, int index, char valor[])
{

  if (index < 0 || index > longitud(pLista) - 1)
  {
    printf("Posición ilegal. Como el Aston Martin de Fernando\n");
    exit(-1);
  }
  else
  {
    TNodo *insercion = malloc(sizeof(TNodo));
    if (index == 0)
    {
      insercion->valor = malloc(strlen(valor)*sizeof(char));
      strcpy(insercion->valor, valor);
      insercion->pSiguiente = pLista->pPrimero;
      pLista->pPrimero = insercion;
    }
    else
    {
      TNodo *nodoiterativo = malloc(sizeof(TNodo));
      nodoiterativo = pLista->pPrimero;

      for (int i = 1; i <= index; i++)
      {
        nodoiterativo = nodoiterativo->pSiguiente;
      }
      insercion->valor = nodoiterativo->valor;
      insercion->pSiguiente = nodoiterativo->pSiguiente;
      nodoiterativo->pSiguiente = insercion;
      nodoiterativo->valor = valor;
    }
  }
}

//  Elimina el prime  r elemento de la lista.
//! AHORA MISMO ELIMINA EL ULTIMO.
// TODO COMENTARIO TO GUAPO AHI A LO GUAY --PABLO
void eliminar(TLista *pLista)
{
  //! BORRA EL PRIMERO

  // TNodo *elim = pLista->pPrimero;
  // pLista->pPrimero = pLista->pPrimero->pSiguiente;
  // free(elim);
  // tamaño--;

  //! BORRA EL ÚLTIMO

  if (longitud(pLista) == 1)
  {
    // free(pLista->pPrimero);
    // pLista->pPrimero = NULL;
    // pLista->pUltimo = NULL;
    destruir(pLista);
  }
  else
  {
    TNodo *pElim;
    for (pElim = pLista->pPrimero; pElim->pSiguiente != pLista->pUltimo; pElim = pElim->pSiguiente)
      ;
    free(pLista->pUltimo->valor);
    pLista->pUltimo = pElim;
    pElim->pSiguiente = NULL;
    free(pElim->pSiguiente);
  }
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
void eliminarN(TLista *pLista, int index)
{
  if (index < 0 || index > longitud(pLista) - 1)
  {
    printf("Posición ilegal. Como el Red Bull de Verstappen\n");
    exit(-1);
  }
  else
  {
    TNodo *borrar = NULL;

    if (index == 0)
    {
      borrar = pLista->pPrimero;
      free(borrar->valor);
      pLista->pPrimero = pLista->pPrimero->pSiguiente;
      free(borrar);
    }
    else
    {
      TNodo *nodoiterativo = pLista->pPrimero;
      for (int i = 1; i < index; i++)
      {
        if (nodoiterativo->pSiguiente == NULL)
        {
          printf("Roto al intentar eliminar el supuesto elemento %d\n", index);
          exit(-1);
        }
        nodoiterativo = nodoiterativo->pSiguiente;
      }
      borrar = nodoiterativo->pSiguiente;
      free(borrar->valor);
      nodoiterativo->pSiguiente = borrar->pSiguiente;
      if (borrar->pSiguiente == NULL)
      { // este es el ultimo elemento
        pLista->pUltimo = nodoiterativo;
      }
      else
      {
        borrar->pSiguiente = NULL;
      }
      free(borrar);
    }
  }
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY
char *getElementoN(TLista *pLista, int index)
{
  if (index < 0 || index > (longitud(pLista) - 1))
  {
    printf("Posición ilegal. Como el Mercedes de Hamilton\n");
    exit(-1);
  }
  TNodo *aux = pLista->pPrimero;
  for (int i = 0; i < index; i++)
    aux = aux->pSiguiente;
  return aux->valor;
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY --PABLO
/*
 * Para imprimir los elementos de la lista le pasamos el puntero con el nombre de la lista
 * y mediante un bucle for se va imprimiendo el valor contenido en cada posición de la lista,
 * desde la primera hasta la última. Esto se logra gracias a un nodo iterador, que se inicializa
 * en la primera posición y va iterando a su siguiente elemento (a través de pSiguiente) hasta
 * que el último elemento sea NULL (fin de la lista).
 */
void imprimir(TLista *pLista)
{
  for (TNodo *pIterador = pLista->pPrimero; pIterador != NULL; pIterador = pIterador->pSiguiente)
    printf("[ %s ]", pIterador->valor);
  printf("\n");
}

// TODO COMENTARIO TO GUAPO AHI A LO GUAY -PABLO
int longitud(TLista *pLista)
{
  int tam = 0;
  for (TNodo *pIterador = pLista->pPrimero; pIterador != NULL; pIterador = pIterador->pSiguiente)
  {
    tam += 1;
  }
  return tam;
}
