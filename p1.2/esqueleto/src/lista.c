#include <stdio.h>
#include <stdlib.h>
#include <lista.h>
#include <string.h>

/*
 * Para crear una lista, le pasamos un puntero con el nombre de la lista y un valor entero como primer elemento.
 * Una vez pasados los parámetros, se crea la lista con un único nodo, que será el primer y último elemento de la lista,
 * pues se acaba de crear ésta. Debido a esto se emplea asignación en memoria del nodo y valor del nodo.
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
 * Para destruir la lista, limpiamos los espacios de memoria de cada nodo de forma iterativa empleando dos estructuras nodo.
 * El primer nodo empieza apuntando al primer elemento de la lista mientras que el segundo nodo apuntará al siguiente elemento
 * para cada iteración. Así, vamos liberando la lista desde el primer elemento hasta el último.
 * Cuando el primer puntero apunta a NULL, se libera el espacio de memoria de la lista y finaliza el proceso.
 */

void destruir(TLista *pLista)
{
  TNodo *pAux1;
  struct Nodo *pAux2;

  for (pAux1 = pLista->pPrimero; pAux1 != NULL;)
  {
    pAux2 = pAux1->pSiguiente;
    //? free(pAux1->valor);
    free(pAux1);
    pAux1 = pAux2;
  }
  free(pLista);
}

/*
 * Para insertar un elemento a la lista, le pasamos el puntero con el nombre de la lista y un entero que
 * contiene el valor a insertar. Se crea un puntero de tipo TNodo con asignación de memoria de tamaño equivalente
 * al de TNodo. Despues le damos el valor introducido y le asignamos como siguiente elemento el primero de la Lista.
 */
void insertar(TLista *pLista, char valor[])
{
  TNodo *nuevo = malloc(sizeof(TNodo));
  nuevo->valor = malloc(strlen(valor)*sizeof(char));
  strcpy(nuevo->valor, valor);
  nuevo->pSiguiente = pLista->pPrimero;
  pLista->pPrimero = nuevo;
}

/*
 * Para insertar un elemento al final de la lista, le pasamos el puntero con el nombre de la lista y un entero que
 * contiene el valor a insertar. Se crea un puntero de tipo TNodo con asignación de memoria de tamaño equivalente al de TNodo.
 * Le damos el valor introducido, le asignamos como elemento siguiente del ultimo elemento de la lista el elemento nuevo y a la
 * lista le asignamos como último elemento el creado.
*/

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

/*
 * Para insertar un valor cualquiera en una posición N de la lista, suponemos que lo insertamos detras de N. Una vez recogidos los
 * parámetros, se controla que el índice no sea negativo o sobrepase el ínidce del último elemento. Con índice válido, se declaran dos
 * nodos auxiliares, uno iterativo de avance y otro "clon" que nos ayudará a insertarlo en la lista. Despús, discriminamos index:
 *    - Para index = 0 --> insertar(pLista, valor)
 *    - Para index >= 1 avanzamos en la lista hasta colocarnos en la posición "index", donde usaremos el nodo clon para recoger la información
 *      del nodo iterativo. Así podremos redirigir estos nodos, por lo que se habría simulado una inserción.
 */

void insertarN(TLista *pLista, int index, char valor[])
{

  if (index < 0 || index > longitud(pLista) - 1)
  {
    fprintf(stderr,"Posición ilegal. No se puede acceder a una zona fuera de la lista. Finalizando el programa.\n");
    exit(-1);
  }
  else
  {
    if (index == 0)
    {
      insertar(pLista,valor);
    }
    else
    {
      TNodo *nodoiterativo = malloc(sizeof(TNodo));
      TNodo *clon = malloc(sizeof(TNodo));
      nodoiterativo = pLista->pPrimero;

      for (int i = 1; i <= index; i++)
      {
        nodoiterativo = nodoiterativo->pSiguiente;
      }
      clon->valor = nodoiterativo->valor;
      clon->pSiguiente = nodoiterativo->pSiguiente;
      nodoiterativo->pSiguiente = clon;
      nodoiterativo->valor = valor;
    }
  }
}

/*
 * Para eliminar el último elemento de la lista, le pasamos el puntero con el nombre de la lista en primer lugar.
 * Si la pila sólo tiene un elemento, se llama al método destruir. Si tiene más, se recorre la lista con un bucle for
 * que va a colocar a un puntero pElim justo antes del último. Luego se libera el espacio de memoria del valor del último
 * elemento y actualizamos pUltimo, a pElim.
*/
void eliminar(TLista *pLista)
{
  if (longitud(pLista) == 1)
  {
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

/*
 * Para elminar un elemento en concreto de la lista, le pasamos el puntero con el nombre de la lista y un entero con el índice
 * del elemento a eliminar. Primero se controla si el índice es negativo o mayor al índice del último elemento. Tras esto se crea
 * un puntero "borrar" de tipo TNodo que apunta a NULL. Si el índice es 0, se elimina el primer elemento de la lista. Si es >=1,
 * se declara un nodo iterativo que recorre la lista hasta almacenar la informacion del elemento deseado. Si iterando la lista se
 * llega a una zona de memoria no permitida, salta una excepción que finaliza el programa. Una vez finalizada la iteración se libera 
 * el nodo deseado, reordenando la lista para que no haya espacios libres entre elementos.
*/
void eliminarN(TLista *pLista, int index)
{
  if (index < 0 || index > longitud(pLista) - 1)
  {
    fprintf(stderr,"Posición ilegal. No se puede acceder a una zona fuera de la lista. Finalizando el programa.\n");
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

/*
 * Para obtener un elemento de la lista, le pasamos el puntero con el nombre de la lista y un entero que 
 * contiene el índice a obtener. Comprobamos si el índice está dentro del rango de la lista y en caso contrario,
 * salta una excepción que finaliza el programa. Mediante un bucle for recorremos la lista hasta llegar al índice deseado
 * y guardamos en un puntero auxiliar el nodo. Por último devolvemos el valor almacenado en dicho nodo.
*/
char *getElementoN(TLista *pLista, int index)
{
  if (index < 0 || index > (longitud(pLista) - 1))
  {
    fprintf(stderr,"Posición ilegal. No se puede acceder a una zona fuera de la lista. Finalizando el programa.\n");
    exit(-1);
  }
  TNodo *aux = pLista->pPrimero;
  for (int i = 0; i < index; i++)
    aux = aux->pSiguiente;
  return aux->valor;
}

/* 
 * Para imprimir la lista primero vamos a pasarle el puntero con el nombre de la lista y con un bucle
 * for se va a imprimir el índice y el valor de cada elemento desde el primero hasta el último.
 */
void imprimir(TLista *pLista)
{
  int i=0;
  for (TNodo *pIterador = pLista->pPrimero; pIterador != NULL; pIterador = pIterador->pSiguiente)
    printf("[ %d | %s ]", i++, pIterador->valor);
  printf("\n");
}

/*
 * Para obtener la longitud de la lista le vamos a pasar el puntero con el nombre de la lista.
 * Usamos un bucle for para recorrer toda la lista y mientras se va recorriendo se le suma 1
 * unidad entera a una variable llamada 'tam'. Por último devolvemos dicha variable.
*/
int longitud(TLista *pLista)
{
  int tam = 0;
  for (TNodo *pIterador = pLista->pPrimero; pIterador != NULL; pIterador = pIterador->pSiguiente)
  {
    tam += 1;
  }
  return tam;
}
