#include "sort.h"
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================
 * BUBBLE SORT
 * ============================================================
 */

/**
 * sort_bubble – implementação do Bubble Sort com snapshots.
 *
 * A cada troca, chama snapshot(vetor, n, j, j+1, ctx) para registrar
 * o estado atual do vetor. O número de snapshots é proporcional ao
 * número de trocas realizadas.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
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
                    snapshot(vetor, n, j, j + 1, ctx);
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

/*
 * ============================================================
 * SELECTION SORT
 * ============================================================
 */

/**
 * sort_selection – implementação do Selection Sort com snapshots.
 *
 * A cada seleção do mínimo e troca, chama snapshot(vetor, n, i, min_idx, ctx).
 * O número de snapshots é igual ao número de trocas realizadas.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
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
                snapshot(vetor, n, i, min, ctx);
            void *tmp = vetor[i];
            vetor[i] = vetor[min];
            vetor[min] = tmp;
        }
    }
}

/*
 * ============================================================
 * INSERTION SORT
 * ============================================================
 */

/**
 * sort_insertion – implementação do Insertion Sort com snapshots.
 *
 * Otimização: snapshot apenas APÓS inserir a chave (um por iteração),
 * em vez de a cada deslocamento. Isso reduz drasticamente o número
 * de frames sem perder a essência visual do algoritmo.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
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
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
        /* Snapshot após a inserção, mostrando a nova posição da chave */
        if (snapshot)
            snapshot(vetor, n, j + 1, -1, ctx);
    }
}

/*
 * ============================================================
 * SHELL SORT
 * ============================================================
 */

/**
 * sort_shell – implementação do Shell Sort com snapshots.
 *
 * Usa a sequência de gaps de Knuth (1, 4, 13, 40, ...).
 * Otimização: snapshot após cada inserção (um por iteração do gap),
 * em vez de a cada deslocamento.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
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
                vetor[j] = vetor[j - gap];
                j -= gap;
            }
            vetor[j] = chave;
            /* Snapshot após cada inserção */
            if (snapshot)
                snapshot(vetor, n, j, -1, ctx);
        }
        gap /= 3;
    }
}

/*
 * ============================================================
 * QUICK SORT
 * ============================================================
 */

/**
 * particiona – função auxiliar do Quick Sort.
 *
 * Usa o último elemento como pivô. A cada troca durante o
 * particionamento, chama snapshot(vetor, n, i, j, ctx).
 */
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

/**
 * quick_recursivo – função recursiva do Quick Sort.
 */
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

/**
 * sort_quick – implementação do Quick Sort com snapshots.
 *
 * A cada troca durante o particionamento, chama snapshot(vetor, n, i, j, ctx).
 * O número de snapshots é proporcional ao número de trocas.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
void sort_quick(void **vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void *ctx)
{
    if (n <= 1)
        return;
    quick_recursivo(vetor, 0, n - 1, n, cmp, snapshot, ctx);
}

/*
 * ============================================================
 * MERGE SORT
 * ============================================================
 */

/**
 * merge – intercala dois subvetores ordenados.
 *
 * OTIMIZAÇÃO CRÍTICA: snapshot apenas UMA VEZ por intercalação,
 * e não para cada elemento copiado. Isso reduz o número de frames
 * de O(N·log N) para O(N), mantendo a animação compreensível.
 *
 * @param vetor Vetor contendo os subvetores a intercalar.
 * @param lo    Índice inicial do primeiro subvetor.
 * @param mid   Índice final do primeiro subvetor.
 * @param hi    Índice final do segundo subvetor.
 * @param n     Tamanho total do vetor.
 * @param cmp   Função de comparação.
 * @param snapshot Callback para gerar frames.
 * @param ctx   Contexto passado para snapshot.
 */
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

    /* Copia de volta para o vetor original */
    for (int m = 0; m < tam; m++)
        vetor[lo + m] = tmp[m];

    /* ÚNICO snapshot por merge – destaca o intervalo intercalado */
    if (snapshot)
        snapshot(vetor, n, lo, hi, ctx);

    free(tmp);
}

/**
 * merge_recursivo – função recursiva do Merge Sort.
 */
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

/**
 * sort_merge – implementação do Merge Sort com snapshots.
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 */
void sort_merge(void **vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void *ctx)
{
    if (n <= 1)
        return;
    merge_recursivo(vetor, 0, n - 1, n, cmp, snapshot, ctx);
}