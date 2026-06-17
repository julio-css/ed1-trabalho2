#include "sort.h"
#include <stdlib.h>
#include <string.h>

/* =============================================================
 * AUXILIAR INTERNA
 * ============================================================= */

/*
 * troca — troca dois ponteiros no vetor e chama o snapshot.
 * Centraliza a troca para nao repetir o mesmo bloco em cada algoritmo.
 */
static void troca(void** vetor, int i, int j,
                  FuncaoSnapshot snapshot, void* ctx) {
    void* tmp   = vetor[i];
    vetor[i]    = vetor[j];
    vetor[j]    = tmp;
    if (snapshot) snapshot(vetor, 0, i, j, ctx);
    /* n e passado como 0 aqui pois cada algoritmo passa o n correto
       diretamente nas chamadas onde conhece o tamanho total */
}


/* =============================================================
 * BUBBLE SORT
 *
 * Percorre o vetor repetidamente, comparando pares adjacentes
 * e trocando quando necessario. A cada passagem, o maior elemento
 * "borbulha" para o final. Para quando nao ha trocas em uma passagem.
 * ============================================================= */

void sort_bubble(void** vetor, int n,
                 FuncaoComparacaoSort cmp,
                 FuncaoSnapshot snapshot, void* ctx) {
    for (int i = 0; i < n - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (cmp(vetor[j], vetor[j+1]) > 0) {
                void* tmp    = vetor[j];
                vetor[j]     = vetor[j+1];
                vetor[j+1]   = tmp;
                trocou = 1;
                if (snapshot) snapshot(vetor, n, j, j+1, ctx);
            }
        }
        /* se nao trocou nada, o vetor ja esta ordenado */
        if (!trocou) break;
    }
}


/* =============================================================
 * SELECTION SORT
 *
 * A cada iteracao, encontra o menor elemento do trecho nao ordenado
 * e o coloca na posicao correta. Sempre faz n-1 trocas no maximo.
 * ============================================================= */

void sort_selection(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx) {
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            if (cmp(vetor[j], vetor[min]) < 0)
                min = j;
        }
        /* so troca (e gera frame) se o minimo nao e a propria posicao */
        if (min != i) {
            void* tmp  = vetor[i];
            vetor[i]   = vetor[min];
            vetor[min] = tmp;
            if (snapshot) snapshot(vetor, n, i, min, ctx);
        }
    }
}


/* =============================================================
 * INSERTION SORT
 *
 * Mantem um subvetor ordenado a esquerda. A cada passo, pega o
 * proximo elemento e o insere na posicao correta dentro do subvetor,
 * deslocando os maiores para a direita.
 * ============================================================= */

void sort_insertion(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx) {
    for (int i = 1; i < n; i++) {
        void* chave = vetor[i];
        int j = i - 1;
        while (j >= 0 && cmp(vetor[j], chave) > 0) {
            vetor[j+1] = vetor[j];
            if (snapshot) snapshot(vetor, n, j, j+1, ctx);
            j--;
        }
        vetor[j+1] = chave;
    }
}


/* =============================================================
 * SHELL SORT
 *
 * Versao generalizada do Insertion Sort que compara elementos
 * distantes por um gap, reduzindo o gap ate 1.
 * Usa a sequencia de Knuth: 1, 4, 13, 40, ...  (h = 3*h + 1)
 * ============================================================= */

void sort_shell(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx) {
    /* calcula o maior gap de Knuth menor que n */
    int gap = 1;
    while (gap < n / 3) gap = 3 * gap + 1;

    while (gap >= 1) {
        /* insertion sort com passo gap */
        for (int i = gap; i < n; i++) {
            void* chave = vetor[i];
            int j = i;
            while (j >= gap && cmp(vetor[j - gap], chave) > 0) {
                vetor[j] = vetor[j - gap];
                if (snapshot) snapshot(vetor, n, j, j - gap, ctx);
                j -= gap;
            }
            vetor[j] = chave;
        }
        gap /= 3;
    }
}


/* =============================================================
 * QUICK SORT
 *
 * Divide o vetor em torno de um pivo (ultimo elemento) e ordena
 * recursivamente as duas metades. O particioamento e in-place.
 * ============================================================= */

/*
 * particiona — reorganiza o vetor[lo..hi] em torno do pivo (vetor[hi]).
 * Retorna o indice final do pivo.
 */
static int particiona(void** vetor, int lo, int hi, int n,
                      FuncaoComparacaoSort cmp,
                      FuncaoSnapshot snapshot, void* ctx) {
    void* pivo = vetor[hi];
    int i = lo - 1;

    for (int j = lo; j < hi; j++) {
        if (cmp(vetor[j], pivo) <= 0) {
            i++;
            void* tmp  = vetor[i];
            vetor[i]   = vetor[j];
            vetor[j]   = tmp;
            if (snapshot) snapshot(vetor, n, i, j, ctx);
        }
    }
    /* coloca o pivo na posicao correta */
    void* tmp    = vetor[i+1];
    vetor[i+1]   = vetor[hi];
    vetor[hi]    = tmp;
    if (snapshot) snapshot(vetor, n, i+1, hi, ctx);

    return i + 1;
}

static void quick_recursivo(void** vetor, int lo, int hi, int n,
                             FuncaoComparacaoSort cmp,
                             FuncaoSnapshot snapshot, void* ctx) {
    if (lo < hi) {
        int p = particiona(vetor, lo, hi, n, cmp, snapshot, ctx);
        quick_recursivo(vetor, lo,   p-1, n, cmp, snapshot, ctx);
        quick_recursivo(vetor, p+1,  hi,  n, cmp, snapshot, ctx);
    }
}

void sort_quick(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx) {
    if (n <= 1) return;
    quick_recursivo(vetor, 0, n-1, n, cmp, snapshot, ctx);
}


/* =============================================================
 * MERGE SORT
 *
 * Divide o vetor ao meio recursivamente e intercala os subvetores
 * ordenados. Usa um buffer auxiliar temporario para o merge.
 * ============================================================= */

static void merge(void** vetor, int lo, int mid, int hi, int n,
                  FuncaoComparacaoSort cmp,
                  FuncaoSnapshot snapshot, void* ctx) {
    int tam = hi - lo + 1;
    void** tmp = (void**) malloc(tam * sizeof(void*));
    if (!tmp) return;

    int i = lo, j = mid + 1, k = 0;

    /* intercala os dois subvetores no buffer */
    while (i <= mid && j <= hi) {
        if (cmp(vetor[i], vetor[j]) <= 0)
            tmp[k++] = vetor[i++];
        else
            tmp[k++] = vetor[j++];
    }
    while (i <= mid)  tmp[k++] = vetor[i++];
    while (j <= hi)   tmp[k++] = vetor[j++];

    /* copia de volta para o vetor original, gerando um frame por elemento */
    for (int m = 0; m < tam; m++) {
        vetor[lo + m] = tmp[m];
        if (snapshot) snapshot(vetor, n, lo + m, -1, ctx);
    }

    free(tmp);
}

static void merge_recursivo(void** vetor, int lo, int hi, int n,
                             FuncaoComparacaoSort cmp,
                             FuncaoSnapshot snapshot, void* ctx) {
    if (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        merge_recursivo(vetor, lo,    mid, n, cmp, snapshot, ctx);
        merge_recursivo(vetor, mid+1, hi,  n, cmp, snapshot, ctx);
        merge(vetor, lo, mid, hi, n, cmp, snapshot, ctx);
    }
}

void sort_merge(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx) {
    if (n <= 1) return;
    merge_recursivo(vetor, 0, n-1, n, cmp, snapshot, ctx);
}   