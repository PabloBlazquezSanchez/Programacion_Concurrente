#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

int main(int argc, char *argv[])
{

  /*CREO LA LISTA*/
  TLista *pLista;

  pLista = malloc(sizeof(TLista));

  crear(pLista, "100");

  /*AÑADO ELEMENTOS A LA LISTA*/
  insertar(pLista, "50");
  insertarFinal(pLista, "200");

  insertarN(pLista, 0, "1");
  insertarN(pLista, 2, "150");
  insertarFinal(pLista, "250");

  imprimir(pLista);
  printf("1// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  /*SE LOS QUITO*/
  eliminar(pLista);

  imprimir(pLista);
  printf("2// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  eliminarN(pLista, 2);

  imprimir(pLista);
  printf("3// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  eliminar(pLista);
  eliminar(pLista);
  eliminar(pLista);

  imprimir(pLista);
  printf("4// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  insertarFinal(pLista, "1000");
  insertarFinal(pLista, "2000");
  insertarFinal(pLista, "3000");
  insertarFinal(pLista, "4000");
  insertarFinal(pLista, "5000");
  insertarFinal(pLista, "6000");
  insertarFinal(pLista, "7000");

  eliminarN(pLista, 6);
  imprimir(pLista);
  printf("5// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  eliminarN(pLista, 0);
  imprimir(pLista);
  printf("6// %s---%s , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));
  /*OBTENGO SUS ELEMENTOS*/
  printf("(%s) ", getElementoN(pLista, 0));
  printf("(%s) ", getElementoN(pLista, 2));
  printf("(%s) ", getElementoN(pLista, 3));

  /*BORRO LA LISTA*/
  destruir(pLista); // Lo que va después si se ejecuta ocasiona una segmentation fault
  // imprimir(pLista);
  // printf("%d---%d , %d\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));
  return 0;
}