#include <stdlib.h>
#include "lista.h"

/*
 * ============================================================
 * ESTRUTURAS INTERNAS
 * ============================================================
 */

/**
 * No – célula da lista duplamente encadeada.
 *
 * Além do ponteiro genérico para o dado, possui links para o
 * próximo e para o anterior, permitindo navegação bidirecional.
 * Isso facilita remoções e inserções em ambas as extremidades.
 */
typedef struct No
{
    void *dado;
    struct No *ant;
    struct No *prox;
} No;

/**
 * Lista – cabeçalho da lista duplamente encadeada.
 *
 * Mantém ponteiros para o início e fim, além do tamanho atual.
 * Ter o ponteiro para o fim permite inserções O(1) no final,
 * e ter o ponteiro para o início permite inserções O(1) no início.
 */
struct Lista
{
    No *inicio;
    No *fim;
    int tamanho;
};

/*
 * ============================================================
 * CONSTRUTOR E DESTRUIDOR
 * ============================================================
 */

/**
 * lista_criar – aloca e inicializa uma nova lista vazia.
 *
 * @return Ponteiro para a nova lista, ou NULL em caso de falha.
 */
Lista *lista_criar(void)
{
    Lista *l = (Lista *)malloc(sizeof(Lista));
    if (!l)
        return NULL;
    l->inicio = l->fim = NULL;
    l->tamanho = 0;
    return l;
}

/**
 * lista_destruir – libera toda a memória da lista.
 *
 * Percorre a lista nó por nó, liberando a memória de cada 'No'
 * antes de avançar para o próximo. Ao final, libera o header.
 *
 * @note Esta função NÃO libera os dados armazenados.
 *       O usuário deve liberar os dados antes de destruir a lista.
 */
void lista_destruir(Lista *l)
{
    if (!l)
        return;
    No *atual = l->inicio;
    while (atual)
    {
        No *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(l);
}

/*
 * ============================================================
 * OPERAÇÕES DE INSERÇÃO
 * ============================================================
 */

/**
 * lista_inserir_fim – insere um elemento no final da lista.
 *
 * Graças ao ponteiro 'l->fim', não precisamos percorrer a lista
 * toda para inserir no final. A complexidade é O(1).
 *
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int lista_inserir_fim(Lista *l, void *dado)
{
    if (!l)
        return 0;
    No *novo = (No *)malloc(sizeof(No));
    if (!novo)
        return 0;

    novo->dado = dado;
    novo->prox = NULL;
    novo->ant = l->fim;

    if (l->fim)
        l->fim->prox = novo;
    else
        l->inicio = novo; /* Lista estava vazia */

    l->fim = novo;
    l->tamanho++;
    return 1;
}

/**
 * lista_insere_inicio – insere um elemento no início da lista.
 *
 * Também é O(1), atualizando apenas o ponteiro 'l->inicio'.
 *
 * @return 1 em caso de sucesso, 0 em caso de falha.
 */
int lista_insere_inicio(Lista *l, void *dado)
{
    if (!l)
        return 0;
    No *novo = (No *)malloc(sizeof(No));
    if (!novo)
        return 0;

    novo->dado = dado;
    novo->ant = NULL;
    novo->prox = l->inicio;

    if (l->inicio)
        l->inicio->ant = novo;
    else
        l->fim = novo; /* Lista estava vazia */

    l->inicio = novo;
    l->tamanho++;
    return 1;
}

/*
 * ============================================================
 * OPERAÇÃO DE REMOÇÃO
 * ============================================================
 */

/**
 * lista_remove – remove um nó que contém exatamente o ponteiro fornecido.
 *
 * Como é uma lista duplamente encadeada, uma vez encontrado o nó,
 * a remoção é simples pois temos acesso direto ao vizinho anterior
 * e ao próximo para "religar" a lista.
 *
 * @return Ponteiro para o dado removido, ou NULL se não encontrado.
 */
void *lista_remove(Lista *l, void *dado)
{
    if (!l)
        return NULL;
    No *atual = l->inicio;

    while (atual)
    {
        if (atual->dado == dado)
        {
            /* Ajusta o link do nó anterior */
            if (atual->ant)
                atual->ant->prox = atual->prox;
            else
                l->inicio = atual->prox; /* Era o primeiro nó */

            /* Ajusta o link do nó próximo */
            if (atual->prox)
                atual->prox->ant = atual->ant;
            else
                l->fim = atual->ant; /* Era o último nó */

            void *ret = atual->dado;
            free(atual);
            l->tamanho--;
            return ret;
        }
        atual = atual->prox;
    }
    return NULL;
}

/*
 * ============================================================
 * ACESSO POR ÍNDICE
 * ============================================================
 */

/**
 * lista_get – acessa um dado pelo índice.
 *
 * Diferente de um array, o acesso aqui é O(n), pois precisamos
 * percorrer a lista até chegar na posição desejada.
 *
 * @param idx Índice do elemento (0 = primeiro).
 * @return Ponteiro para o dado, ou NULL se o índice for inválido.
 */
void *lista_get(Lista *l, int idx)
{
    if (!l || idx < 0 || idx >= l->tamanho)
        return NULL;
    No *atual = l->inicio;
    for (int i = 0; i < idx; i++)
    {
        atual = atual->prox;
    }
    return atual->dado;
}

/*
 * ============================================================
 * CONSULTAS DE ESTADO
 * ============================================================
 */

/**
 * lista_tamanho – retorna o número de elementos na lista.
 */
int lista_tamanho(Lista *l)
{
    return l ? l->tamanho : 0;
}

/**
 * lista_vazia – verifica se a lista está vazia.
 *
 * @return 1 (verdadeiro) se vazia, 0 (falso) caso contrário.
 */
int lista_vazia(Lista *l)
{
    return l ? (l->tamanho == 0) : 1;
}