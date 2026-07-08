#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "arvore.h"
#include "forma.h"

/**
 * @defgroup geo Geo
 * @brief Módulo de suporte geométrico e leitura do arquivo .geo.
 *
 * Responsabilidades:
 *   - Calcular propriedades geométricas das formas (área, largura, altura)
 *   - Fornecer comparadores para a ABB e para os algoritmos de ordenação
 *   - Ler o arquivo .geo e popular a árvore
 *
 * Separado do main.c porque depende de forma.h —
 * a árvore é genérica e não pode incluir forma.h diretamente.
 *
 * @see forma.h, arvore.h
 * @{
 */

/* ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================ */

/**
 * @brief Calcula a área de uma forma conforme as regras do trabalho.
 *
 * Regras para cada tipo:
 * - Círculo   : π * r²
 * - Retângulo : w * h
 * - Linha     : 1.5 * comprimento
 * - Texto     : 10.0 * número de caracteres
 * - Polígono  : 0
 *
 * @param f Ponteiro para a forma.
 *
 * @return Área calculada (double).
 *
 * @pre f != NULL.
 */
double geo_area(Forma* f);

/**
 * @brief Retorna a largura lógica de uma forma.
 *
 * Regras para cada tipo:
 * - Retângulo : w
 * - Círculo   : 2 * r
 * - Linha     : |x2 - x1|
 * - Texto     : 1.0 * número de caracteres
 * - Polígono  : 0
 *
 * @param f Ponteiro para a forma.
 *
 * @return Largura calculada (double).
 *
 * @pre f != NULL.
 */
double geo_largura(Forma* f);

/**
 * @brief Retorna a altura lógica de uma forma.
 *
 * Regras para cada tipo:
 * - Retângulo : h
 * - Círculo   : 2 * r
 * - Linha     : 1.5 (conforme especificação)
 * - Texto     : 10.0 (conforme especificação)
 * - Polígono  : 0
 *
 * @param f Ponteiro para a forma.
 *
 * @return Altura calculada (double).
 *
 * @pre f != NULL.
 */
double geo_altura(Forma* f);

/* ============================================================
 * COMPARADORES
 * ============================================================ */

/**
 * @brief Comparação padrão para a ABB (ordem default).
 *
 * Chave: y crescente, depois x crescente, depois área crescente.
 *
 * @param f1 Primeira forma (void*).
 * @param f2 Segunda forma (void*).
 *
 * @return <0 se f1 < f2, 0 se iguais, >0 se f1 > f2.
 *
 * @pre f1 != NULL, f2 != NULL.
 * @see geo_comparar_area, geo_comparar_largura, geo_comparar_altura, geo_comparar_cor
 */
int geo_comparar(void* f1, void* f2);

/**
 * @brief Comparador para ordenação por área crescente.
 *
 * Desempate pela chave padrão (y, x, área).
 *
 * @param f1 Primeira forma (void*).
 * @param f2 Segunda forma (void*).
 *
 * @return <0 se f1 < f2, 0 se iguais, >0 se f1 > f2.
 *
 * @pre f1 != NULL, f2 != NULL.
 */
int geo_comparar_area(void* f1, void* f2);

/**
 * @brief Comparador para ordenação por largura crescente.
 *
 * Desempate pela chave padrão (y, x, área).
 *
 * @param f1 Primeira forma (void*).
 * @param f2 Segunda forma (void*).
 *
 * @return <0 se f1 < f2, 0 se iguais, >0 se f1 > f2.
 *
 * @pre f1 != NULL, f2 != NULL.
 */
int geo_comparar_largura(void* f1, void* f2);

/**
 * @brief Comparador para ordenação por altura crescente.
 *
 * Desempate pela chave padrão (y, x, área).
 *
 * @param f1 Primeira forma (void*).
 * @param f2 Segunda forma (void*).
 *
 * @return <0 se f1 < f2, 0 se iguais, >0 se f1 > f2.
 *
 * @pre f1 != NULL, f2 != NULL.
 */
int geo_comparar_altura(void* f1, void* f2);

/**
 * @brief Comparador para ordenação por cor de preenchimento.
 *
 * Ordena pela cor de preenchimento em ordem alfabética.
 * Para linhas, usa a cor da borda (conforme especificação).
 * Desempate pela chave padrão (y, x, área).
 *
 * @param f1 Primeira forma (void*).
 * @param f2 Segunda forma (void*).
 *
 * @return <0 se f1 < f2, 0 se iguais, >0 se f1 > f2.
 *
 * @pre f1 != NULL, f2 != NULL.
 */
int geo_comparar_cor(void* f1, void* f2);

/* ============================================================
 * UTILITÁRIOS
 * ============================================================ */

/**
 * @brief Extrai o ID de uma Forma passada como void*.
 *
 * Usada como callback em buscarPorIdArvore para manter
 * a árvore desacoplada do módulo Forma.
 *
 * @param elemento Ponteiro para uma Forma (void*).
 *
 * @return ID da forma (int).
 *
 * @pre elemento != NULL.
 * @see arvore.h
 */
int geo_get_id(void* elemento);

/* ============================================================
 * LEITURA DO ARQUIVO .geo
 * ============================================================ */

/**
 * @brief Lê o arquivo .geo linha por linha e insere cada forma na árvore.
 *
 * Usa geo_comparar como chave para a árvore.
 * Suporta os comandos: r (retângulo), c (círculo), l (linha), t (texto),
 * ts (estilo de texto), f (fonte), e comentários (#).
 *
 * @param arq_geo  Ponteiro para o arquivo .geo aberto para leitura.
 * @param formas   Ponteiro para a árvore onde as formas serão inseridas.
 *
 * @pre arq_geo != NULL, formas != NULL.
 * @post Todas as formas válidas do arquivo estão na árvore.
 */
void geo_processa_arquivo(FILE* arq_geo, Arvore formas);

/** @} */ /* end of geo group */

#endif /* GEO_H */