#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

int main(int argc, char *argv[])
{

  /*CREO LA LISTA - */
  TLista *pLista;

  pLista = malloc(sizeof(TLista));

  crear(pLista, 100);

  /*AÑADO ELEMENTOS A LA LISTA*/
  insertar(pLista, 50);
  insertarFinal(pLista, 200);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  insertarN(pLista, 0, 0);
  insertarN(pLista, 2, 150);
  insertarFinal(pLista, 250);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  /*SE LOS QUITO*/
  eliminar(pLista);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  eliminarN(pLista, 2);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  eliminar(pLista);
  eliminar(pLista);
  eliminar(pLista);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  for (int i = 50; i <= 300; i += 50)
  {
    insertarFinal(pLista, i);
  }
  eliminarN(pLista, 6);

  imprimir(pLista);
  printf("%d---%d , %d\n\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));

  /*OBTENGO SUS ELEMENTOS*/
  printf("(%d) ", getElementoN(pLista, 0));
  printf("(%d) ", getElementoN(pLista, 2));
  printf("(%d) ", getElementoN(pLista, 3));

  /*BORRO LA LISTA*/
  destruir(pLista); //Lo que va después si se ejecuta ocasiona una segmentation fault
  // imprimir(pLista);
  // printf("%d---%d , %d\n", pLista->pPrimero->valor, pLista->pUltimo->valor, longitud(pLista));
  return 0;
}