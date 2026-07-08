#ifndef CIRCULO_H
#define CIRCULO_H

#include "forma.h"

/**
 * @defgroup circulo Circulo
 * @brief Operações específicas para formas geométricas do tipo FORMA_CIRCULO.
 *
 * Este módulo implementa as funções de criação, acesso a atributos e
 * cálculos geométricos para círculos. Todas as funções assumem que o
 * ponteiro @p f passado como parâmetro é estritamente do tipo FORMA_CIRCULO.
 *
 * @warning As funções não verificam o tipo da forma para fins de performance.
 *          O chamador deve garantir que a forma seja um círculo.
 * @see forma.h
 * @ingroup forma
 * @{
 */

/**
 * @brief Cria uma nova instância de um círculo geométrico.
 *
 * Aloca e inicializa uma estrutura Forma com os dados de um círculo.
 *
 * @param id          Identificador único do círculo (>= 1).
 * @param x           Coordenada X do centro.
 * @param y           Coordenada Y do centro.
 * @param r           Raio do círculo (> 0).
 * @param cor_borda   Cor da borda (string válida, não NULL).
 * @param cor_preench Cor do preenchimento (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma criada, ou NULL em caso de falha
 *         de alocação de memória.
 *
 * @pre id >= 1, r > 0, cor_borda != NULL, cor_preench != NULL.
 * @post Retorna um ponteiro válido para uma Forma do tipo FORMA_CIRCULO,
 *       ou NULL se falhar.
 */
Forma* circulo_cria(int id, double x, double y, double r,
                    char* cor_borda, char* cor_preench);

/**
 * @brief Obtém o valor do raio do círculo.
 *
 * @param f Ponteiro para a forma (deve ser FORMA_CIRCULO e não NULL).
 *
 * @return Valor do raio (double).
 *
 * @pre f != NULL e f é do tipo FORMA_CIRCULO.
 */
double circulo_get_raio(Forma* f);

/**
 * @brief Calcula a área ocupada pelo círculo.
 *
 * A área é calculada como π * r².
 *
 * @param f Ponteiro para a forma (deve ser FORMA_CIRCULO e não NULL).
 *
 * @return Área do círculo (double).
 *
 * @pre f != NULL e f é do tipo FORMA_CIRCULO.
 */
double circulo_get_area(Forma* f);

/**
 * @brief Verifica se uma coordenada (px, py) está contida na área do círculo.
 *
 * Útil para operações de seleção espacial (como o comando 'sel').
 * Retorna 1 se o ponto estiver dentro ou exatamente na borda do círculo.
 *
 * @param f  Ponteiro para a forma (deve ser FORMA_CIRCULO e não NULL).
 * @param px Coordenada X do ponto a testar.
 * @param py Coordenada Y do ponto a testar.
 *
 * @return 1 (verdadeiro) se o ponto estiver contido, 0 (falso) caso contrário.
 *
 * @pre f != NULL e f é do tipo FORMA_CIRCULO.
 */
int circulo_contem_ponto(Forma* f, double px, double py);

/** @} */ /* end of circulo group */

#endif /* CIRCULO_H */