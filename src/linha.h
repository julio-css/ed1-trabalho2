#ifndef LINHA_H
#define LINHA_H

#include "forma.h"

/**
 * @defgroup linha Linha
 * @brief Operações específicas para formas do tipo FORMA_LINHA.
 *
 * Uma linha é definida por duas extremidades: (x1, y1) e (x2, y2).
 * A âncora armazenada em Forma (x, y) é o ponto (x1, y1).
 * O segundo ponto (x2, y2) é guardado nos dados específicos da linha.
 *
 * @note Todas as funções assumem que o ponteiro @p f passado é
 *       estritamente do tipo FORMA_LINHA.
 *
 * @see forma.h
 * @ingroup forma
 * @{
 */

/* ============================================================
 * CONSTRUTOR
 * ============================================================ */

/**
 * @brief Cria uma forma do tipo linha.
 *
 * @param id Identificador único da forma (>= 1).
 * @param x1 Coordenada X da primeira extremidade.
 * @param y1 Coordenada Y da primeira extremidade.
 * @param x2 Coordenada X da segunda extremidade.
 * @param y2 Coordenada Y da segunda extremidade.
 * @param cor Cor da linha (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, cor != NULL.
 * @post Retorna uma Forma do tipo FORMA_LINHA.
 */
Forma* linha_cria(int id, double x1, double y1, double x2, double y2,
                  char* cor);

/* ============================================================
 * GETTERS
 * ============================================================ */

/**
 * @brief Obtém a coordenada X da segunda extremidade.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_LINHA).
 *
 * @return Coordenada X2 (double).
 *
 * @pre f != NULL e f é do tipo FORMA_LINHA.
 */
double linha_get_x2(Forma* f);

/**
 * @brief Obtém a coordenada Y da segunda extremidade.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_LINHA).
 *
 * @return Coordenada Y2 (double).
 *
 * @pre f != NULL e f é do tipo FORMA_LINHA.
 */
double linha_get_y2(Forma* f);

/* ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================ */

/**
 * @brief Calcula o comprimento euclidiano da linha.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_LINHA).
 *
 * @return Comprimento da linha (double).
 *
 * @pre f != NULL e f é do tipo FORMA_LINHA.
 */
double linha_get_comprimento(Forma* f);

/** @} */ /* end of linha group */

#endif /* LINHA_H */