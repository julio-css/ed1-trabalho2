#ifndef SORT_H
#define SORT_H

/**
 * @defgroup sort Sort
 * @brief Algoritmos de ordenação com geração de frames SVG (snapshots).
 *
 * Cada algoritmo opera sobre um vetor de ponteiros void* e aceita:
 *   - uma função de comparação (mesma assinatura de FuncaoComparacao)
 *   - um callback de snapshot chamado a cada iteração relevante
 *
 * O callback é o mecanismo que desacopla os algoritmos da geração de SVG:
 * quem chama a ordenação decide o que fazer a cada passo — pode gerar
 * um arquivo SVG, imprimir no terminal, ou não fazer nada (NULL).
 *
 * @see qry.h, svg.h
 * @{
 */

/* ============================================================
 * TIPOS
 * ============================================================ */

/**
 * @brief Função de comparação entre dois elementos do vetor.
 *
 * @param a Primeiro elemento (void*).
 * @param b Segundo elemento (void*).
 *
 * @return Negativo se a < b, zero se iguais, positivo se a > b.
 *
 * @pre a != NULL, b != NULL.
 */
typedef int (*FuncaoComparacaoSort)(void* a, void* b);

/**
 * @brief Função de snapshot chamada a cada iteração relevante do algoritmo.
 *
 * @param vetor Estado atual do vetor de ponteiros.
 * @param n     Tamanho do vetor.
 * @param i     Índice sendo comparado/trocado (ou -1 se não aplicável).
 * @param j     Índice sendo comparado/trocado (ou -1 se não aplicável).
 * @param ctx   Contexto livre passado pelo chamador (ex: struct CtxFrames).
 *
 * @pre vetor != NULL, n > 0.
 */
typedef void (*FuncaoSnapshot)(void** vetor, int n, int i, int j, void* ctx);

/* ============================================================
 * ALGORITMOS DE ORDENAÇÃO
 * ============================================================ */

/**
 * @brief Bubble Sort.
 *
 * A cada troca, chama snapshot(vetor, n, j, j+1, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 */
void sort_bubble(void** vetor, int n,
                 FuncaoComparacaoSort cmp,
                 FuncaoSnapshot snapshot, void* ctx);

/**
 * @brief Selection Sort.
 *
 * A cada seleção do mínimo e troca, chama snapshot(vetor, n, i, min_idx, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 */
void sort_selection(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx);

/**
 * @brief Insertion Sort.
 *
 * A cada deslocamento de elemento, chama snapshot(vetor, n, j, j+1, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 */
void sort_insertion(void** vetor, int n,
                    FuncaoComparacaoSort cmp,
                    FuncaoSnapshot snapshot, void* ctx);

/**
 * @brief Shell Sort.
 *
 * Usa a sequência de gaps de Knuth (1, 4, 13, 40, ...).
 * A cada movimento de elemento, chama snapshot(vetor, n, j, j+gap, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 */
void sort_shell(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

/**
 * @brief Quick Sort.
 *
 * Usa o último elemento como pivô.
 * A cada troca durante o particionamento, chama snapshot(vetor, n, i, j, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 */
void sort_quick(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

/**
 * @brief Merge Sort.
 *
 * A cada intercalação (merge), chama snapshot(vetor, n, lo, hi, ctx).
 *
 * @param vetor    Vetor de ponteiros a ser ordenado.
 * @param n        Número de elementos.
 * @param cmp      Função de comparação.
 * @param snapshot Callback para gerar frames (pode ser NULL).
 * @param ctx      Contexto passado para snapshot.
 *
 * @pre vetor != NULL, n >= 0, cmp != NULL.
 * @post vetor ordenado em ordem crescente segundo cmp.
 *
 * @note A implementação atual gera um snapshot por merge, não por elemento,
 *       para reduzir a quantidade de frames.
 */
void sort_merge(void** vetor, int n,
                FuncaoComparacaoSort cmp,
                FuncaoSnapshot snapshot, void* ctx);

/** @} */ /* end of sort group */

#endif /* SORT_H */