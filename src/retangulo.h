#ifndef RETANGULO_H
#define RETANGULO_H

#include "forma.h"

/**
 * @defgroup retangulo Retangulo
 * @brief Operações específicas para formas do tipo FORMA_RETANGULO.
 *
 * A âncora do retângulo é o canto inferior esquerdo (x, y).
 * O retângulo se estende de (x, y) até (x + w, y + h).
 *
 * @note Todas as funções assumem que o ponteiro @p f passado é
 *       estritamente do tipo FORMA_RETANGULO.
 *
 * @see forma.h
 * @ingroup forma
 * @{
 */

/* ============================================================
 * CONSTRUTOR
 * ============================================================ */

/**
 * @brief Cria uma forma do tipo retângulo.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X do canto inferior esquerdo.
 * @param y            Coordenada Y do canto inferior esquerdo.
 * @param w            Largura do retângulo (> 0).
 * @param h            Altura do retângulo (> 0).
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, w > 0, h > 0, cor_borda != NULL, cor_preench != NULL.
 * @post Retorna uma Forma do tipo FORMA_RETANGULO.
 */
Forma* retangulo_cria(int id, double x, double y, double w, double h,
                      char* cor_borda, char* cor_preench);

/* ============================================================
 * GETTERS
 * ============================================================ */

/**
 * @brief Obtém a largura do retângulo.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_RETANGULO).
 *
 * @return Largura (double).
 *
 * @pre f != NULL e f é do tipo FORMA_RETANGULO.
 */
double retangulo_get_largura(Forma* f);

/**
 * @brief Obtém a altura do retângulo.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_RETANGULO).
 *
 * @return Altura (double).
 *
 * @pre f != NULL e f é do tipo FORMA_RETANGULO.
 */
double retangulo_get_altura(Forma* f);

/* ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================ */

/**
 * @brief Calcula a área do retângulo (w * h).
 *
 * @param f Ponteiro para a forma (deve ser FORMA_RETANGULO).
 *
 * @return Área (double).
 *
 * @pre f != NULL e f é do tipo FORMA_RETANGULO.
 */
double retangulo_get_area(Forma* f);

/**
 * @brief Verifica se um ponto (px, py) está dentro do retângulo.
 *
 * @param f  Ponteiro para a forma (deve ser FORMA_RETANGULO).
 * @param px Coordenada X do ponto.
 * @param py Coordenada Y do ponto.
 *
 * @return 1 (verdadeiro) se o ponto está dentro, 0 (falso) caso contrário.
 *
 * @pre f != NULL e f é do tipo FORMA_RETANGULO.
 */
int retangulo_contem_ponto(Forma* f, double px, double py);

/** @} */ /* end of retangulo group */

#endif /* RETANGULO_H */