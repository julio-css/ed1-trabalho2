#ifndef TEXTO_H
#define TEXTO_H

#include "forma.h"

/**
 * @defgroup texto Texto
 * @brief Operações específicas para formas do tipo FORMA_TEXTO.
 *
 * Um texto é posicionado em (x, y) com âncora definida por ancora_tipo.
 *
 * Posições da âncora:
 * - 'i' (início) : o texto começa em (x, y)
 * - 'm' (meio)   : o centro do texto está em (x, y)
 * - 'f' (fim)    : o texto termina em (x, y)
 *
 * @note Todas as funções assumem que o ponteiro @p f passado é
 *       estritamente do tipo FORMA_TEXTO.
 *
 * @see forma.h
 * @ingroup forma
 * @{
 */

/* ============================================================
 * CONSTRUTOR
 * ============================================================ */

/**
 * @brief Cria uma forma do tipo texto.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X da âncora.
 * @param y            Coordenada Y da âncora.
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 * @param ancora_tipo  Posição da âncora: 'i' (início), 'm' (meio), 'f' (fim).
 * @param conteudo     Conteúdo do texto (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, cor_borda != NULL, cor_preench != NULL, conteudo != NULL.
 * @pre ancora_tipo deve ser 'i', 'm' ou 'f'.
 * @post Retorna uma Forma do tipo FORMA_TEXTO.
 */
Forma* texto_cria(int id, double x, double y,
                  char* cor_borda, char* cor_preench,
                  char ancora_tipo, char* conteudo);

/** @} */ /* end of texto group */

#endif /* TEXTO_H */