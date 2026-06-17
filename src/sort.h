#ifndef SORT_H
#define SORT_H

/*
 * sort.h — algoritmos de ordenacao com geracao de frames SVG
 *
 * Cada algoritmo opera sobre um vetor de ponteiros void* e aceita:
 *   - uma funcao de comparacao (mesma assinatura de FuncaoComparacao)
 *   - um callback de snapshot chamado a cada iteracao relevante
 *
 * O callback e o mecanismo que desacopla os algoritmos da geracao de SVG:
 * quem chama a ordenacao decide o que fazer a cada passo — pode gerar
 * um arquivo SVG, imprimir no terminal, ou nao fazer nada (NULL).
 *
 * Conforme o trabalho, os algoritmos implementados sao:
 *   ss — Selection Sort
 *   bs — Bubble Sort
 *   is — Insertion Sort
 *   shs — Shell Sort
 *   qs — Quick Sort
 *   ms — Merge Sort
 */

/*
 * FuncaoComparacaoSort — compara dois elementos do vetor.
 * Retorna negativo se a < b, zero se iguais, positivo se a > b.
 */
typedef int (*FuncaoComparacaoSort)(void* a, void* b);

/*
 * FuncaoSnapshot — chamada a cada iteracao relevante do algoritmo.
 *
 * vetor : estado atual do vetor de ponteiros
 * n     : tamanho do vetor
 * i, j  : indices sendo comparados ou trocados neste passo
 *          (-1 se nao aplicavel neste tipo de passo)
 * ctx   : contexto livre passado pelo chamador (ex: struct com caminho
 *         base do SVG e contador de frames)
 *
 * pre-condicao: vetor != NULL, n > 0
 */
typedef void (*FuncaoSnapshot)(void** vetor, int n, int i, int j, void* ctx);

/*
 * sort_bubble — Bubble Sort.
 * A cada troca, chama snapshot(vetor, n, i, i+1, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_bubble(void** vetor, int n,
                 FuncaoComparacaoSort cmp,
                 FuncaoSnapshot snapshot, void* ctx);

/*
 * sort_selection — Selection Sort.
 * A cada selecao do minimo e troca, chama snapshot(vetor, n, i, min_idx, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_selection(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx);

/*
 * sort_insertion — Insertion Sort.
 * A cada deslocamento de elemento, chama snapshot(vetor, n, j, j+1, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_insertion(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx);

/*
 * sort_shell — Shell Sort.
 * Usa a sequencia de gaps de Knuth (1, 4, 13, 40, ...).
 * A cada movimento de elemento, chama snapshot(vetor, n, j, j+gap, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_shell(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

/*
 * sort_quick — Quick Sort.
 * Usa o ultimo elemento como pivo.
 * A cada troca durante o particioamento, chama snapshot(vetor, n, i, j, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_quick(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

/*
 * sort_merge — Merge Sort.
 * A cada elemento copiado de volta no merge, chama snapshot(vetor, n, i, -1, ctx).
 *
 * pre-condicao: vetor != NULL, n >= 0, cmp != NULL
 * pos-condicao: vetor ordenado em ordem crescente segundo cmp
 */
void sort_merge(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

#endif