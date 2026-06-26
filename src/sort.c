#include "sort.h"
#include <stdlib.h>
#include <string.h>

/* =============================================================
 * BUBBLE SORT
 *
 * Percorre o vetor repetidamente comparando pares adjacentes
 * e trocando quando necessario. Para cedo se nao houver trocas.
 * ============================================================= */

void sort_bubble(void **vetor, int n,
                 FuncaoComparacaoSort cmp,
                 FuncaoSnapshot snapshot, void *ctx)
{
    for (int i = 0; i < n - 1; i++)
    {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; j++)
        {
            if (cmp(vetor[j], vetor[j + 1]) > 0)
            {
                if (snapshot)
                    snapshot(vetor, n, j, j + 1, ctx); // Snapshot antes da troca
                void *tmp = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou)
            break;
    }
}

/* =============================================================
 * SELECTION SORT
 *
 * A cada iteracao encontra o menor do trecho nao ordenado
 * e coloca na posicao correta.
 * ============================================================= */

void sort_selection(void **vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void *ctx)
{
    for (int i = 0; i < n - 1; i++)
    {
        int min = i;
        for (int j = i + 1; j < n; j++)
        {
            if (cmp(vetor[j], vetor[min]) < 0)
                min = j;
        }
        if (min != i)
        {
            if (snapshot)
                snapshot(vetor, n, i, min, ctx); // Snapshot antes da troca
            void *tmp = vetor[i];
            vetor[i] = vetor[min];
            vetor[min] = tmp;
        }
    }
}

/* =============================================================
 * INSERTION SORT
 *
 * Mantem subvetor ordenado a esquerda. Insere cada elemento
 * na posicao correta deslocando os maiores para a direita.
 * ============================================================= */

void sort_insertion(void **vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void *ctx)
{
    for (int i = 1; i < n; i++)
    {
        void *chave = vetor[i];
        int j = i - 1;
        while (j >= 0 && cmp(vetor[j], chave) > 0)
        {
            if (snapshot)
                snapshot(vetor, n, j, j + 1, ctx); // Snapshot antes de deslocar
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }
}

/* =============================================================
 * SHELL SORT
 *
 * Insertion Sort generalizado com gaps decrescentes.
 * Usa sequencia de Knuth: 1, 4, 13, 40, ...
 * ============================================================= */

void sort_shell(void **vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void *ctx)
{
    int gap = 1;
    while (gap < n / 3)
        gap = 3 * gap + 1;

    while (gap >= 1)
    {
        for (int i = gap; i < n; i++)
        {
            void *chave = vetor[i];
            int j = i;
            while (j >= gap && cmp(vetor[j - gap], chave) > 0)
            {
                if (snapshot)
                    snapshot(vetor, n, j, j - gap, ctx); // Snapshot antes de mover
                vetor[j] = vetor[j - gap];
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
 * Divide em torno de um pivo (ultimo elemento) e ordena
 * recursivamente. Particionamento in-place.
 * ============================================================= */

static int particiona(void **vetor, int lo, int hi, int n,
                      FuncaoComparacaoSort cmp,
                      FuncaoSnapshot snapshot, void *ctx)
{
    void *pivo = vetor[hi];
    int i = lo - 1;

    for (int j = lo; j < hi; j++)
    {
        if (cmp(vetor[j], pivo) <= 0)
        {
            i++;
            if (snapshot)
                snapshot(vetor, n, i, j, ctx);
            void *tmp = vetor[i];
            vetor[i] = vetor[j];
            vetor[j] = tmp;
        }
    }
    if (snapshot)
        snapshot(vetor, n, i + 1, hi, ctx);
    void *tmp = vetor[i + 1];
    vetor[i + 1] = vetor[hi];
    vetor[hi] = tmp;
    return i + 1;
}

static void quick_recursivo(void **vetor, int lo, int hi, int n,
                            FuncaoComparacaoSort cmp,
                            FuncaoSnapshot snapshot, void *ctx)
{
    if (lo < hi)
    {
        int p = particiona(vetor, lo, hi, n, cmp, snapshot, ctx);
        quick_recursivo(vetor, lo, p - 1, n, cmp, snapshot, ctx);
        quick_recursivo(vetor, p + 1, hi, n, cmp, snapshot, ctx);
    }
}

void sort_quick(void **vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void *ctx)
{
    if (n <= 1)
        return;
    quick_recursivo(vetor, 0, n - 1, n, cmp, snapshot, ctx);
}

/* =============================================================
 * MERGE SORT
 *
 * Divide ao meio recursivamente e intercala os subvetores.
 * Usa buffer auxiliar temporario para o merge.
 * ============================================================= */

static void merge(void **vetor, int lo, int mid, int hi, int n,
                  FuncaoComparacaoSort cmp,
                  FuncaoSnapshot snapshot, void *ctx)
{
    int tam = hi - lo + 1;
    void **tmp = (void **)malloc(tam * sizeof(void *));
    if (!tmp)
        return;

    int i = lo, j = mid + 1, k = 0;
    while (i <= mid && j <= hi)
    {
        if (cmp(vetor[i], vetor[j]) <= 0)
            tmp[k++] = vetor[i++];
        else
            tmp[k++] = vetor[j++];
    }
    while (i <= mid)
        tmp[k++] = vetor[i++];
    while (j <= hi)
        tmp[k++] = vetor[j++];

    for (int m = 0; m < tam; m++)
    {
        vetor[lo + m] = tmp[m];
        if (snapshot)
            snapshot(vetor, n, lo + m, -1, ctx);
    }
    free(tmp);
}

static void merge_recursivo(void **vetor, int lo, int hi, int n,
                            FuncaoComparacaoSort cmp,
                            FuncaoSnapshot snapshot, void *ctx)
{
    if (lo < hi)
    {
        int mid = lo + (hi - lo) / 2;
        merge_recursivo(vetor, lo, mid, n, cmp, snapshot, ctx);
        merge_recursivo(vetor, mid + 1, hi, n, cmp, snapshot, ctx);
        merge(vetor, lo, mid, hi, n, cmp, snapshot, ctx);
    }
}

void sort_merge(void **vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void *ctx)
{
    if (n <= 1)
        return;
    merge_recursivo(vetor, 0, n - 1, n, cmp, snapshot, ctx);
}