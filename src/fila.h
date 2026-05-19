#ifndef FILA_H
#define FILA_H

/* capacidade maxima da fila circular estatica */
#define FILA_MAX 100

/*
 * Mddulo Fila (Queue - FIFO)
 
 * Implementacao de uma fila circular estatica baseada em ponteiros genericos (void*).
 * O primeiro elemento a entrar eh o primeiro a sair (First In, First Out).
 * Utiliza um array estatico interno para evitar a sobrecarga de 'malloc' a cada insercao.
 * A struct interna e opaca (definida apenas no .c).
 */
typedef struct Fila Fila;

/*
 * inicializa a estrutura da fila.
 * pos-condicao: retorna um ponteiro para a nova Fila criada com tamanho 0.
 * retorna NULL se houver falha na alocaçao de memoria.
 */
Fila* fila_cria();

/*
 * liberta a memria alocada para a estrutura da fila.
 * lembrando que esta funçao NAO liberta a memoria dos dados (void*) que estao
 * guardados dentro da fila. eh responsabilidade do utilizador dar 'free' nos dados.
 */
void fila_destroi(Fila* f);

/*
 * insere um novo elemento no fim da fila.
 * pre-condiçao: f != NULL e fila nao pode estar cheia.
 * pos-condiçao: retorna 1 em caso de sucesso, ou 0 se a fila estiver cheia.
 */
int fila_insere(Fila* f, void* dado);

/*
 * remove e devolve o elemento mais antigo (o que esta na frente) da fila.
 * pre-condiçao: f != NULL e fila nao pode estar vazia.
 * pos-condiçao: retorna o ponteiro (void*) do dado removido, ou NULL se vazia.
 */
void* fila_remove(Fila* f);

/*
 * consulta o elemento mais antigo da fila sem o remover.
 * pos-condiçao: retorna o ponteiro (void*) do dado da frente, ou NULL se vazia.
 */
void* fila_frente(Fila* f);

/*
 * permite pegar a um elemento específico da fila como se fosse um array,
   sem removê-lo. o índice 0 representa o elemento da frente (mais antigo).
 * pre-condiçao: idx >= 0 e idx < quantidade atual de elementos.
 * pos-condiçao: retorna o ponteiro (void*) do dado, ou NULL se indice invalido.
 */
void* fila_get(Fila* f, int idx);

/* retorna a quantidade de elementos atualmente na fila. */
int fila_tamanho(Fila* f);

/* retorna 1 (Verdadeiro) se a fila estiver vazia, 0 caso contrário. */
int fila_vazia(Fila* f);

/* retorna 1 (Verdadeiro) se a fila atingiu a capacidade FILA_MAX, 0 caso contrário. */
int fila_cheia(Fila* f);

/* esvazia a fila logicamente (zera os indices e contador), 
   mas sem libertar os dados armazenados na memoria.
 */
void fila_limpa(Fila* f);

#endif