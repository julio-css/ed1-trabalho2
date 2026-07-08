#include "arvore.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * ============================================================
 * ESTRUTURAS INTERNAS (privadas ao modulo)
 * ============================================================
 */

/**
 * No – célula básica da árvore binária.
 *
 * 'info' é um ponteiro genérico para o dado do usuário (ex: Forma*).
 * Os ponteiros esq/dir apontam para os filhos esquerdo e direito.
 */
typedef struct no
{
    void *info;
    struct no *esq;
    struct no *dir;
} No;

/**
 * ArvoreImpl – cabeçalho da árvore.
 *
 * Guarda a raiz e a função de comparação escolhida na criação.
 * O typedef void* Arvore do .h aponta para esta struct,
 * mantendo a implementação opaca para o usuário.
 */
typedef struct arvore_impl
{
    No *raiz;
    FuncaoComparacao comparar;
} ArvoreImpl;

/**
 * CtxBuscaId – contexto auxiliar para buscarPorIdNo.
 *
 * Permite comunicar o resultado entre chamadas recursivas
 * sem usar variáveis globais. A busca é interrompida assim
 * que o elemento é encontrado.
 */
typedef struct
{
    int id_buscado;
    void *encontrado;
    int (*getid)(void *);
} CtxBuscaId;

/*
 * ============================================================
 * FUNÇÕES AUXILIARES (recursivas e de manipulação de nós)
 * ============================================================
 */

/**
 * criarNo – aloca e inicializa um novo nó.
 *
 * Retorna NULL se a alocação falhar.
 * O nó é criado sem filhos (esq = dir = NULL).
 */
static No *criarNo(void *elemento)
{
    No *novo = (No *)malloc(sizeof(No));
    if (novo != NULL)
    {
        novo->info = elemento;
        novo->esq = NULL;
        novo->dir = NULL;
    }
    return novo;
}

/**
 * inserirNo – insere recursivamente na árvore.
 *
 * Menor vai para a esquerda, maior ou igual vai para a direita.
 * Elementos com chave igual vão para a direita por convenção,
 * garantindo que todos os inseridos sejam armazenados mesmo com chaves
 * duplicadas (ex: dois retângulos com a mesma área).
 */
static No *inserirNo(No *raiz, void *elemento, FuncaoComparacao comp)
{
    if (raiz == NULL)
        return criarNo(elemento);

    int res = comp(elemento, raiz->info);
    if (res < 0)
        raiz->esq = inserirNo(raiz->esq, elemento, comp);
    else
        raiz->dir = inserirNo(raiz->dir, elemento, comp);

    return raiz;
}

/**
 * buscarNo – busca recursiva por uma chave.
 *
 * Retorna o ponteiro para o dado se encontrado, ou NULL caso contrário.
 * A busca segue a mesma lógica da inserção.
 */
static void *buscarNo(No *raiz, void *chave, FuncaoComparacao comp)
{
    if (raiz == NULL)
        return NULL;

    int res = comp(chave, raiz->info);
    if (res == 0)
        return raiz->info;
    if (res < 0)
        return buscarNo(raiz->esq, chave, comp);
    return buscarNo(raiz->dir, chave, comp);
}

/**
 * encontrarMinimo – retorna o nó mais à esquerda de uma subárvore.
 *
 * O menor elemento de uma subárvore é sempre o nó mais à esquerda.
 * Usado na remoção de nós com dois filhos (sucessor in-order).
 */
static No *encontrarMinimo(No *raiz)
{
    while (raiz->esq != NULL)
        raiz = raiz->esq;
    return raiz;
}

/**
 * removerNo – remove recursivamente um nó da árvore.
 *
 * Caso 1: folha ou 1 filho – substitui pelo filho existente.
 * Caso 2: 2 filhos – copia o sucessor in-order (menor da direita)
 *         para o nó atual e remove o sucessor do lugar de origem.
 *
 * O parâmetro 'infoRemovido' é preenchido apenas uma vez (primeiro match)
 * para não ser sobrescrito durante a remoção do sucessor no Caso 2.
 * Isso é importante porque no Caso 2 chamamos removerNo recursivamente
 * e não queremos perder o ponteiro do elemento original que está sendo
 * removido.
 */
static No *removerNo(No *raiz, void *chave, FuncaoComparacao comp,
                     void **infoRemovido)
{
    if (raiz == NULL)
        return NULL;

    int res = comp(chave, raiz->info);

    if (res < 0)
    {
        raiz->esq = removerNo(raiz->esq, chave, comp, infoRemovido);
    }
    else if (res > 0)
    {
        raiz->dir = removerNo(raiz->dir, chave, comp, infoRemovido);
    }
    else
    {
        /* Guarda o dado apenas na primeira vez que encontra o nó */
        if (*infoRemovido == NULL)
            *infoRemovido = raiz->info;

        /* Caso 1: nó folha ou com um único filho */
        if (raiz->esq == NULL)
        {
            No *temp = raiz->dir;
            free(raiz);
            return temp;
        }
        else if (raiz->dir == NULL)
        {
            No *temp = raiz->esq;
            free(raiz);
            return temp;
        }

        /* Caso 2: nó com dois filhos – substitui pelo sucessor */
        No *suc = encontrarMinimo(raiz->dir);
        raiz->info = suc->info;
        raiz->dir = removerNo(raiz->dir, suc->info, comp, infoRemovido);
    }
    return raiz;
}

/*
 * ============================================================
 * TRAVESSIAS E PERCURSOS
 * ============================================================
 */

/**
 * emOrdemNo – percorre a árvore em ordem simétrica.
 *
 * Visita: esquerda, raiz, direita.
 * Esta ordem garante que os elementos sejam visitados
 * em ordem crescente segundo a função de comparação.
 */
static void emOrdemNo(No *raiz, FuncaoVisita funcVisita, void *aux)
{
    if (raiz == NULL)
        return;
    emOrdemNo(raiz->esq, funcVisita, aux);
    funcVisita(raiz->info, aux);
    emOrdemNo(raiz->dir, funcVisita, aux);
}

/**
 * emOrdemFiltroNo – travessia em ordem com predicado.
 *
 * Chama funcVisita apenas nos elementos para os quais
 * predicado(elemento, ctx) retorna 1 (verdadeiro).
 * Útil para operações de seleção espacial, como o comando 'sel'.
 */
static void emOrdemFiltroNo(No *raiz,
                            FuncaoPredicado predicado, void *ctx,
                            FuncaoVisita funcVisita, void *aux)
{
    if (raiz == NULL)
        return;
    emOrdemFiltroNo(raiz->esq, predicado, ctx, funcVisita, aux);
    if (predicado(raiz->info, ctx))
        funcVisita(raiz->info, aux);
    emOrdemFiltroNo(raiz->dir, predicado, ctx, funcVisita, aux);
}

/**
 * buscarPorIdNo – percorre a árvore até encontrar um ID específico.
 *
 * Para a recursão assim que encontra (ctx->encontrado != NULL),
 * evitando percorrer o restante da árvore desnecessariamente.
 * Usa uma função callback para extrair o ID do elemento,
 * mantendo a árvore desacoplada do tipo específico de dado.
 */
static void buscarPorIdNo(No *raiz, CtxBuscaId *ctx)
{
    if (raiz == NULL || ctx->encontrado != NULL)
        return;
    buscarPorIdNo(raiz->esq, ctx);
    if (ctx->getid(raiz->info) == ctx->id_buscado)
    {
        ctx->encontrado = raiz->info;
        return;
    }
    buscarPorIdNo(raiz->dir, ctx);
}

/*
 * ============================================================
 * UTILITÁRIOS DE MANUTENÇÃO
 * ============================================================
 */

/**
 * contarNos – conta recursivamente o número de nós.
 */
static int contarNos(No *raiz)
{
    if (raiz == NULL)
        return 0;
    return 1 + contarNos(raiz->esq) + contarNos(raiz->dir);
}

/**
 * destruirNo – libera a memória de todos os nós em pós-ordem.
 *
 * A ordem é: esquerda, direita, raiz.
 * Se 'funcDestruir' for fornecida, ela é chamada para cada dado
 * armazenado, permitindo que o usuário libere a memória dos dados
 * (ex: forma_destroi).
 */
static void destruirNo(No *raiz, FuncaoDestruir funcDestruir)
{
    if (raiz == NULL)
        return;
    destruirNo(raiz->esq, funcDestruir);
    destruirNo(raiz->dir, funcDestruir);
    if (funcDestruir != NULL)
        funcDestruir(raiz->info);
    free(raiz);
}

/*
 * ============================================================
 * FUNÇÕES PÚBLICAS (API da árvore)
 * ============================================================
 */

/**
 * criarArvore – cria uma nova árvore binária de busca.
 *
 * A função de comparação é obrigatória e define a ordem dos elementos.
 * Retorna NULL se a alocação falhar ou se 'funcComp' for NULL.
 */
Arvore criarArvore(FuncaoComparacao funcComp)
{
    if (funcComp == NULL)
        return NULL;

    ArvoreImpl *arv = (ArvoreImpl *)malloc(sizeof(ArvoreImpl));
    if (arv != NULL)
    {
        arv->raiz = NULL;
        arv->comparar = funcComp;
    }
    return (Arvore)arv;
}

/**
 * inserirArvore – insere um novo elemento na árvore.
 *
 * Se 'elemento' for NULL, a função retorna sem fazer nada.
 * A inserção é feita usando a função de comparação da árvore.
 */
void inserirArvore(Arvore arvore, void *elemento)
{
    if (arvore == NULL || elemento == NULL)
        return;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    arv->raiz = inserirNo(arv->raiz, elemento, arv->comparar);
}

/**
 * buscarArvore – busca um elemento pela chave.
 *
 * Retorna o ponteiro para o dado se encontrado, ou NULL caso contrário.
 * A busca usa a mesma função de comparação da árvore.
 */
void *buscarArvore(Arvore arvore, void *chave)
{
    if (arvore == NULL || chave == NULL)
        return NULL;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    return buscarNo(arv->raiz, chave, arv->comparar);
}

/**
 * removerArvore – remove um elemento da árvore pela chave.
 *
 * Retorna o ponteiro para o dado removido, ou NULL se não encontrado.
 * Importante: o dado não é destruído – o usuário é responsável
 * por liberar a memória (ou reutilizar o ponteiro).
 */
void *removerArvore(Arvore arvore, void *chave)
{
    if (arvore == NULL || chave == NULL)
        return NULL;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    void *infoRemovido = NULL;
    arv->raiz = removerNo(arv->raiz, chave, arv->comparar, &infoRemovido);
    return infoRemovido;
}

/**
 * emOrdemArvore – percorre a árvore em ordem simétrica.
 *
 * Visita todos os elementos em ordem crescente.
 * A função 'funcVisita' é chamada para cada elemento,
 * recebendo 'aux' como parâmetro adicional.
 *
 * Exemplo de uso: coletar todas as formas em uma lista.
 */
void emOrdemArvore(Arvore arvore, FuncaoVisita funcVisita, void *aux)
{
    if (arvore == NULL || funcVisita == NULL)
        return;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    emOrdemNo(arv->raiz, funcVisita, aux);
}

/**
 * emOrdemFiltroArvore – percorre a árvore com filtro.
 *
 * Similar a emOrdemArvore, mas só visita os elementos que
 * satisfazem o predicado. Usado para seleções espaciais.
 */
void emOrdemFiltroArvore(Arvore arvore,
                         FuncaoPredicado predicado, void *ctx,
                         FuncaoVisita funcVisita, void *aux)
{
    if (arvore == NULL || predicado == NULL || funcVisita == NULL)
        return;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    emOrdemFiltroNo(arv->raiz, predicado, ctx, funcVisita, aux);
}

/**
 * buscarPorIdArvore – busca um elemento pelo ID.
 *
 * Usa um callback 'funcGetId' para extrair o ID do elemento,
 * mantendo a árvore desacoplada de estruturas específicas como Forma.
 *
 * Esta é uma busca ineficiente (O(N)) pois não usa a chave da árvore,
 * mas é necessária para buscas por ID no comando 'inp'.
 * Para melhor performance, poderíamos ter uma árvore adicional indexada por ID.
 */
void *buscarPorIdArvore(Arvore arvore, int id, int (*funcGetId)(void *))
{
    if (arvore == NULL || funcGetId == NULL)
        return NULL;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    CtxBuscaId ctx = {id, NULL, funcGetId};
    buscarPorIdNo(arv->raiz, &ctx);
    return ctx.encontrado;
}

/**
 * tamanhoArvore – retorna o número de elementos na árvore.
 */
int tamanhoArvore(Arvore arvore)
{
    if (arvore == NULL)
        return 0;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    return contarNos(arv->raiz);
}

/**
 * destruirArvore – libera toda a memória da árvore.
 *
 * Se 'funcDestruir' for fornecida, ela é chamada para cada dado
 * armazenado. Isso permite que o usuário libere os dados antes
 * de a árvore ser destruída.
 *
 * Exemplo: destruirArvore(arvore, forma_destroi);
 */
void destruirArvore(Arvore arvore, FuncaoDestruir funcDestruir)
{
    if (arvore == NULL)
        return;

    ArvoreImpl *arv = (ArvoreImpl *)arvore;
    destruirNo(arv->raiz, funcDestruir);
    free(arv);
}