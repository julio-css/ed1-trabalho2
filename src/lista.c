#include <stdlib.h>
#include "lista.h"

/* * ESTRUTURA DO NÓ
 * Além do ponteiro genérico para o dado, possui o link para o
 * próximo e para o anterior, permitindo a navegação bidirecional.
 */
typedef struct No {
    void* dado;
    struct No* ant;
    struct No* prox;
} No;

/* * ESTRUTURA DA LISTA (Header)
 * Centraliza as informações da lista para evitar percursos desnecessários.
 */
struct Lista {
    No* inicio;
    No* fim;
    int tamanho;
};

Lista* lista_criar(void) {
    Lista* l = (Lista*)malloc(sizeof(Lista));
    if (!l) return NULL;
    l->inicio = l->fim = NULL;
    l->tamanho = 0;
    return l;
}

/* * DESTRUIÇÃO DA LISTA
 * Percorre a lista nó por nó, liberando a memória de cada 'No' 
 * antes de avançar para o próximo. Ao final, libera o header.
 */
void lista_destruir(Lista* l) {
    if (!l) return;
    No* atual = l->inicio;
    while (atual) {
        No* prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(l);
}

/* * INSERÇÃO NO FIM (Push Back)
 * ENGENHARIA: Graças ao ponteiro 'l->fim', não precisamos percorrer a
 * lista toda para inserir no final. A complexidade é O(1).
 */
int lista_inserir_fim(Lista* l, void* dado) {
    if (!l) return 0;
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) return 0;
    
    novo->dado = dado;
    novo->prox = NULL;
    novo->ant = l->fim;
    
    if (l->fim) l->fim->prox = novo;
    else l->inicio = novo; /* Lista estava vazia */
    
    l->fim = novo;
    l->tamanho++;
    return 1;
}

/* * INSERÇÃO NO INÍCIO (Push Front)
 * Também é O(1), atualizando o ponteiro 'l->inicio'.
 */
int lista_insere_inicio(Lista* l, void* dado) {
    if (!l) return 0;
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) return 0;
    
    novo->dado = dado;
    novo->ant = NULL;
    novo->prox = l->inicio;
    
    if (l->inicio) l->inicio->ant = novo;
    else l->fim = novo; /* Lista estava vazia */
    
    l->inicio = novo;
    l->tamanho++;
    return 1;
}

/* * REMOÇÃO POR VALOR
 * ENGENHARIA: Como é uma lista duplamente encadeada, uma vez que
 * encontramos o nó, a remoção é simples pois temos acesso direto ao
 * vizinho anterior e ao próximo para "religar" a lista.
 */
void* lista_remove(Lista* l, void* dado) {
    if (!l) return NULL;
    No* atual = l->inicio;
    
    while (atual) {
        if (atual->dado == dado) {
            /* Ajusta o link do nó anterior */
            if (atual->ant) atual->ant->prox = atual->prox;
            else l->inicio = atual->prox; /* Era o primeiro nó */
            
            /* Ajusta o link do nó próximo */
            if (atual->prox) atual->prox->ant = atual->ant;
            else l->fim = atual->ant; /* Era o último nó */
            
            void* ret = atual->dado;
            free(atual);
            l->tamanho--;
            return ret;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* * ACESSO POR ÍNDICE
 * Diferente de um array, o acesso aqui é O(n), pois precisamos
 * saltar de nó em nó até chegar na posição desejada.
 */
void* lista_get(Lista* l, int idx) {
    if (!l || idx < 0 || idx >= l->tamanho) return NULL;
    No* atual = l->inicio;
    for (int i = 0; i < idx; i++) {
        atual = atual->prox;
    }
    return atual->dado;
}

int lista_tamanho(Lista* l) {
    return l ? l->tamanho : 0;
}

int lista_vazia(Lista* l) {
    return l ? (l->tamanho == 0) : 1;
}