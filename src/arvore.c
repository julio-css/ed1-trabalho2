#include "arvore.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Estrutura de um Nó da Árvore.
 * Fica escondida dentro do .c, garantindo encapsulamento.
 */
typedef struct no {
    void* info;         /* Ponteiro para o dado geométrico/genérico */
    struct no* esq;     /* Ponteiro para o filho à esquerda */
    struct no* dir;     /* Ponteiro para o filho à direita */
} No;

/**
 * Estrutura da Árvore em si.
 * É isso que o "void* Arvore" do .h realmente é por debaixo dos panos.
 */
typedef struct arvore_impl {
    No* raiz;
    FuncaoComparacao comparar; /* Guarda a regra de ordenação escolhida */
} ArvoreImpl;


/* =======================================================
 * FUNÇÕES AUXILIARES (PRIVADAS AO MÓDULO)
 * Usamos 'static' para que não sejam acessíveis de fora.
 * ======================================================= */

static No* criarNo(void* elemento) {
    No* novo = (No*) malloc(sizeof(No));
    if (novo != NULL) {
        novo->info = elemento;
        novo->esq = NULL;
        novo->dir = NULL;
    }
    return novo;
}

static No* inserirNo(No* raiz, void* elemento, FuncaoComparacao comp) {
    /* Caso base: achou o lugar onde inserir */
    if (raiz == NULL) {
        return criarNo(elemento);
    }
    
    int resultado = comp(elemento, raiz->info);
    
    /* Menor vai para a esquerda, maior (ou igual) vai para a direita */
    if (resultado < 0) {
        raiz->esq = inserirNo(raiz->esq, elemento, comp);
    } else {
        /* Se as chaves forem iguais (resultado == 0), por padrão em 
           estruturas espaciais, jogamos para a direita. */
        raiz->dir = inserirNo(raiz->dir, elemento, comp);
    }
    return raiz;
}

static void* buscarNo(No* raiz, void* chave, FuncaoComparacao comp) {
    if (raiz == NULL) return NULL;
    
    int resultado = comp(chave, raiz->info);
    
    if (resultado == 0) return raiz->info; /* Encontrou! */
    if (resultado < 0) return buscarNo(raiz->esq, chave, comp);
    return buscarNo(raiz->dir, chave, comp);
}

/* Encontra o nó mais à esquerda de uma subárvore (o menor valor) */
static No* encontrarMinimo(No* raiz) {
    if (raiz == NULL) return NULL;
    while (raiz->esq != NULL) {
        raiz = raiz->esq;
    }
    return raiz;
}

static No* removerNo(No* raiz, void* chave, FuncaoComparacao comp, void** infoRemovido) {
    if (raiz == NULL) return NULL;
    
    int resultado = comp(chave, raiz->info);
    
    if (resultado < 0) {
        raiz->esq = removerNo(raiz->esq, chave, comp, infoRemovido);
    } else if (resultado > 0) {
        raiz->dir = removerNo(raiz->dir, chave, comp, infoRemovido);
    } else {
        /* Achamos o nó que queremos remover! */
        
        /* Salva o ponteiro original apenas na primeira vez que entra aqui 
           (evita sobrescrever ao remover o sucessor no Caso 2) */
        if (*infoRemovido == NULL) {
            *infoRemovido = raiz->info;
        }
        
        /* Caso 1: É uma folha ou tem apenas 1 filho */
        if (raiz->esq == NULL) {
            No* temp = raiz->dir;
            free(raiz);
            return temp;
        } else if (raiz->dir == NULL) {
            No* temp = raiz->esq;
            free(raiz);
            return temp;
        }
        
        /* Caso 2: Tem 2 filhos. 
           Buscamos o sucessor In-Order (o menor valor da subárvore direita) */
        No* temp = encontrarMinimo(raiz->dir);
        
        /* Copiamos os dados do sucessor para o nó atual */
        raiz->info = temp->info;
        
        /* Removemos o sucessor do lugar original dele */
        raiz->dir = removerNo(raiz->dir, temp->info, comp, infoRemovido);
    }
    return raiz;
}

static void emOrdemNo(No* raiz, FuncaoVisita funcVisita, void* aux) {
    if (raiz != NULL) {
        emOrdemNo(raiz->esq, funcVisita, aux);
        funcVisita(raiz->info, aux); /* Visita a Raiz */
        emOrdemNo(raiz->dir, funcVisita, aux);
    }
}

static void destruirNo(No* raiz, FuncaoDestruir funcDestruir) {
    if (raiz != NULL) {
        /* Destrói em pós-ordem (esquerda, direita, raiz) */
        destruirNo(raiz->esq, funcDestruir);
        destruirNo(raiz->dir, funcDestruir);
        
        if (funcDestruir != NULL) {
            funcDestruir(raiz->info); /* Libera a memória do dado customizado */
        }
        free(raiz); /* Libera a memória do Nó da Árvore */
    }
}


/* =======================================================
 * IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS (Aquelas do arvore.h)
 * ======================================================= */

Arvore criarArvore(FuncaoComparacao funcComp) {
    if (funcComp == NULL) return NULL;
    
    ArvoreImpl* arv = (ArvoreImpl*) malloc(sizeof(ArvoreImpl));
    if (arv != NULL) {
        arv->raiz = NULL;
        arv->comparar = funcComp;
    }
    return (Arvore)arv;
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
    
    return infoRemovido; /* Retorna NULL se a chave não existia */
}

void emOrdemArvore(Arvore arvore, FuncaoVisita funcVisita, void* aux) {
    if (arvore == NULL || funcVisita == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    emOrdemNo(arv->raiz, funcVisita, aux);
}

void destruirArvore(Arvore arvore, FuncaoDestruir funcDestruir) {
    if (arvore == NULL) return;
    ArvoreImpl* arv = (ArvoreImpl*) arvore;
    
    destruirNo(arv->raiz, funcDestruir);
    free(arv); /* Libera a struct de controle da árvore */
}