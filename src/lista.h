#ifndef LISTA_H
#define LISTA_H

typedef struct Lista Lista;

/* cria lista vazia. retorna NULL se falhar */
Lista* lista_criar();

/* libera todos os nos e a estrutura */
void lista_destruir(Lista* l);

/* insere dado no fim. retorna 1 se ok, 0 se falhar */
int lista_inserir_fim(Lista* l, void* dado);

/* insere dado no inicio. retorna 1 se ok, 0 se falhar */
int lista_insere_inicio(Lista* l, void* dado);

/* remove o no com exatamente esse ponteiro. retorna o dado ou NULL */
void* lista_remove(Lista* l, void* dado);

/* acessa dado pelo indice (0 = primeiro). retorna NULL se invalido */
void* lista_get(Lista* l, int idx);

/* retorna quantos elementos tem */
int lista_tamanho(Lista* l);

/* retorna 1 se vazia, 0 caso contrario */
int lista_vazia(Lista* l);

#endif
