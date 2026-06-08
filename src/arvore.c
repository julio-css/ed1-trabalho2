#include "arvore.h"
#include <stdlib.h>
#include <stdio.h>

/* ESTRUTURAS INTERNAS (privadas ao modulo) */

/*
 * No — celula basica da arvore.
 * 'info' e um ponteiro generico para o dado do usuario (ex: Forma*).
 */
typedef struct no {
    void*      info;
    struct no* esq;
    struct no* dir;
} No;

/*
 * ArvoreImpl — cabecalho da arvore.
 * Guarda a raiz e a funcao de comparacao escolhida na criacao.
 * O typedef void* Arvore do .h aponta para esta struct.
 */
typedef struct arvore_impl {
    No* raiz;
    FuncaoComparacao comparar;
} ArvoreImpl;

/*
 * CtxBuscaId — contexto auxiliar para buscarPorIdNo.
 * Permite comunicar o resultado entre chamadas recursivas
 * sem usar variaveis globais.
 */
typedef struct {
    int id_buscado;
    void* encontrado;
    int (*getid)(void*);
} CtxBuscaId;


/* FUNCOES AUXILIARES */

static No* criarNo(void* elemento) {
    No* novo = (No*) malloc(sizeof(No));
    if (novo != NULL) {
        novo->info = elemento;
        novo->esq  = NULL;
        novo->dir  = NULL;
    }
    return novo;
}

/*
 * inserirNo — insere recursivamente.
 * Menor vai para a esquerda, maior ou igual vai para a direita.
 * Elementos com chave igual vao para a direita por convencao,
 * garantindo que todos os inseridos sejam armazenados.
 */
static No* inserirNo(No* raiz, void* elemento, FuncaoComparacao comp) {
    if (raiz == NULL) return criarNo(elemento);

    int res = comp(elemento, raiz->info);
    if (res < 0)
        raiz->esq = inserirNo(raiz->esq, elemento, comp);
    else
        raiz->dir = inserirNo(raiz->dir, elemento, comp);

    return raiz;
}

static void* buscarNo(No* raiz, void* chave, FuncaoComparacao comp) {
    if (raiz == NULL) return NULL;

    int res = comp(chave, raiz->info);
    if (res == 0) return raiz->info;
    if (res < 0)  return buscarNo(raiz->esq, chave, comp);
    return              buscarNo(raiz->dir, chave, comp);
}

/* Retorna o no mais a esquerda de uma subarvore (o menor valor). */
static No* encontrarMinimo(No* raiz) {
    while (raiz->esq != NULL) raiz = raiz->esq;
    return raiz;
}

/*
 * removerNo — remove recursivamente.
 *
 * Caso 1: folha ou 1 filho — substitui pelo filho existente.
 * Caso 2: 2 filhos — copia o sucessor in-order (menor da direita)
 *         para o no atual e remove o sucessor do lugar de origem.
 *
 * O parametro infoRemovido e preenchido apenas uma vez (primeiro match)
 * para nao ser sobrescrito durante a remocao do sucessor no Caso 2.
 */
static No* removerNo(No* raiz, void* chave, FuncaoComparacao comp,
                     void** infoRemovido) {
    if (raiz == NULL) return NULL;

    int res = comp(chave, raiz->info);

    if (res < 0) {
        raiz->esq = removerNo(raiz->esq, chave, comp, infoRemovido);
    } else if (res > 0) {
        raiz->dir = removerNo(raiz->dir, chave, comp, infoRemovido);
    } else {
        /* Guarda o dado apenas na primeira vez que encontra o no */
        if (*infoRemovido == NULL)
            *infoRemovido = raiz->info;

        if (raiz->esq == NULL) {
            No* temp = raiz->dir;
            free(raiz);
            return temp;
        } else if (raiz->dir == NULL) {
            No* temp = raiz->esq;
            free(raiz);
            return temp;
        }
        
        /* 2 filhos: substitui pelo sucessor in-order */
        No* suc   = encontrarMinimo(raiz->dir);
        raiz->info = suc->info;
        raiz->dir  = removerNo(raiz->dir, suc->info, comp, infoRemovido);
    }
    return raiz;
}

/* Travessia em ordem visitando todos os nos. */
static void emOrdemNo(No* raiz, FuncaoVisita funcVisita, void* aux) {
    if (raiz == NULL) return;
    emOrdemNo(raiz->esq, funcVisita, aux);
    funcVisita(raiz->info, aux);
    emOrdemNo(raiz->dir, funcVisita, aux);
}

/*
 * emOrdemFiltroNo — travessia em ordem com predicado.
 * Chama funcVisita apenas nos elementos para os quais
 * predicado(elemento, ctx) retorna 1.
 */
static void emOrdemFiltroNo(No* raiz,
                             FuncaoPredicado predicado, void* ctx,
                             FuncaoVisita funcVisita, void* aux) {
    if (raiz == NULL) return;
    emOrdemFiltroNo(raiz->esq, predicado, ctx, funcVisita, aux);
    if (predicado(raiz->info, ctx))
        funcVisita(raiz->info, aux);
    emOrdemFiltroNo(raiz->dir, predicado, ctx, funcVisita, aux);
}

/*
 * buscarPorIdNo — percorre tudo ate achar o ID.
 * Para a recursao assim que encontra (ctx->encontrado != NULL),
 * evitando percorrer o restante da arvore desnecessariamente.
 */
static void buscarPorIdNo(No* raiz, CtxBuscaId* ctx) {
    if (raiz == NULL || ctx->encontrado != NULL) return;
    buscarPorIdNo(raiz->esq, ctx);
    if (ctx->getid(raiz->info) == ctx->id_buscado) {
        ctx->encontrado = raiz->info;
        return;
    }
    buscarPorIdNo(raiz->dir, ctx);
}

/* Conta os nos recursivamente. */
static int contarNos(No* raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarNos(raiz->esq) + contarNos(raiz->dir);
}

/* Destroi em pos-ordem: esquerda, direita, raiz. */
static void destruirNo(No* raiz, FuncaoDestruir funcDestruir) {
    if (raiz == NULL) return;
    destruirNo(raiz->esq, funcDestruir);
    destruirNo(raiz->dir, funcDestruir);
    if (funcDestruir != NULL)
        funcDestruir(raiz->info);
    free(raiz);
}


/* IMPLEMENTACAO DAS FUNCOES PUBLICAS */

Arvore criarArvore(FuncaoComparacao funcComp) {
    if (funcComp == NULL) return NULL;
    ArvoreImpl* arv = (ArvoreImpl*) malloc(sizeof(ArvoreImpl));
    if (arv != NULL) {
        arv->raiz = NULL;
        arv->comparar = funcComp;
    }
    return (Arvore) arv;
}

void inserirArvore(Arvore arvore, void* elemento) {
    if (arvore == NULL || elemento == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    arv->raiz = inserirNo(arv->raiz, elemento, arv->comparar);
}

void* buscarArvore(Arvore arvore, void* chave) {
    if (arvore == NULL || chave == NULL) return NULL;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    return buscarNo(arv->raiz, chave, arv->comparar);
}

void* removerArvore(Arvore arvore, void* chave) {
    if (arvore == NULL || chave == NULL) return NULL;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    void* infoRemovido = NULL;
    arv->raiz = removerNo(arv->raiz, chave, arv->comparar, &infoRemovido);
    return infoRemovido;
}

void emOrdemArvore(Arvore arvore, FuncaoVisita funcVisita, void* aux) {
    if (arvore == NULL || funcVisita == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    emOrdemNo(arv->raiz, funcVisita, aux);
}

void emOrdemFiltroArvore(Arvore arvore,
                         FuncaoPredicado predicado, void* ctx,
                         FuncaoVisita    funcVisita, void* aux) {
    if (arvore == NULL || predicado == NULL || funcVisita == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    emOrdemFiltroNo(arv->raiz, predicado, ctx, funcVisita, aux);
}

void* buscarPorIdArvore(Arvore arvore, int id, int (*funcGetId)(void*)) {
    if (arvore == NULL || funcGetId == NULL) return NULL;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    CtxBuscaId ctx  = { id, NULL, funcGetId };
    buscarPorIdNo(arv->raiz, &ctx);
    return ctx.encontrado;
}

int tamanhoArvore(Arvore arvore) {
    if (arvore == NULL) return 0;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    return contarNos(arv->raiz);
}

void destruirArvore(Arvore arvore, FuncaoDestruir funcDestruir) {
    if (arvore == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    destruirNo(arv->raiz, funcDestruir);
    free(arv);
}