#ifndef _LISTA_
#define _LISTA_

//* Definición de una estructura con sinónimo: definimos la estructura Nodo (contenido + puntero a siguiente nodo)
typedef struct Nodo
{
  int valor;
  struct Nodo *pSiguiente;
} TNodo; //! La estructura Nodo también se puede definir como TNodo

// Hola esto es un comentario

//* Definimos la estructura 'anónima' TLista que contendrá un puntero al primer nodo (elemento de la lista)
typedef struct
{
  TNodo *pPrimero;
  TNodo *pUltimo;
} TLista;

//Hola
void crear(TLista *pLista, int valor);
void destruir(TLista *pLista);

// 'insertar' inserta al ppio de la lista.
void insertar(TLista *pLista, int valor);
void insertarFinal(TLista *pLista, int valor);
void insertarN(TLista *pLista, int index, int valor);

// 'elimina' elimina el último de la lista.
void eliminar(TLista *pLista);
void eliminarN(TLista *pLista, int index);

int getElementoN(TLista *pLista, int index);

void imprimir(TLista *pLista);
int longitud(TLista *pLista);

#endif
