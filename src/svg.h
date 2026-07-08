#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "lista.h"
#include "forma.h"

/**
 * @defgroup svg SVG
 * @brief Módulo para geração de arquivos .svg a partir de listas de formas.
 *
 * O SVG usa coordenadas com y crescendo para BAIXO.
 * O plano cartesiano do trabalho tem y crescendo para CIMA.
 * A conversão é feita internamente: svg_y = cartesiano_y + MARGEM.
 *
 * As dimensões do SVG são calculadas automaticamente
 * com base nas coordenadas máximas das formas.
 *
 * @see forma.h, lista.h
 * @{
 */

/* ============================================================
 * FUNÇÕES DE DIMENSIONAMENTO
 * ============================================================ */

/**
 * @brief Calcula as dimensões necessárias para o SVG a partir da lista de formas.
 *
 * Percorre a lista e encontra as coordenadas máximas de cada forma.
 * Adiciona uma margem de 20px em cada lado.
 *
 * @param formas  Lista de formas.
 * @param largura Ponteiro para armazenar a largura calculada.
 * @param altura  Ponteiro para armazenar a altura calculada.
 *
 * @pre formas != NULL, largura != NULL, altura != NULL.
 * @post largura e altura são preenchidos com valores > 0.
 */
void svg_calcula_dimensoes(Lista* formas, double* largura, double* altura);

/* ============================================================
 * FUNÇÕES DE ABERTURA E FECHAMENTO
 * ============================================================ */

/**
 * @brief Escreve o cabeçalho do arquivo SVG.
 *
 * @param arq     Ponteiro para o arquivo aberto.
 * @param largura Largura do canvas SVG.
 * @param altura  Altura do canvas SVG.
 *
 * @pre arq != NULL, largura > 0, altura > 0.
 */
void svg_abre(FILE* arq, double largura, double altura);

/**
 * @brief Escreve o cabeçalho do SVG com viewBox personalizada.
 *
 * Permite definir a região visível do SVG, útil para zoom em áreas específicas.
 *
 * @param arq     Ponteiro para o arquivo aberto.
 * @param largura Largura da viewBox.
 * @param altura  Altura da viewBox.
 * @param view_x  Coordenada X do canto superior esquerdo da viewBox.
 * @param view_y  Coordenada Y do canto superior esquerdo da viewBox.
 *
 * @pre arq != NULL, largura > 0, altura > 0.
 */
void svg_abre_com_viewbox(FILE* arq, double largura, double altura,
                          double view_x, double view_y);

/**
 * @brief Escreve o fechamento do arquivo SVG.
 *
 * @param arq Ponteiro para o arquivo aberto.
 *
 * @pre arq != NULL.
 */
void svg_fecha(FILE* arq);

/* ============================================================
 * FUNÇÕES DE DESENHO
 * ============================================================ */

/**
 * @brief Desenha uma forma no arquivo SVG.
 *
 * Faz a conversão de coordenadas do sistema cartesiano para o SVG.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param f      Ponteiro para a forma a ser desenhada.
 * @param altura Altura do canvas (usada para conversão Y).
 *
 * @pre arq != NULL, f != NULL, altura > 0.
 */
void svg_desenha_forma(FILE* arq, Forma* f, double altura);

/**
 * @brief Desenha todas as formas da lista no arquivo SVG.
 *
 * Calcula a altura internamente (chama svg_calcula_dimensoes).
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param formas Lista de formas a serem desenhadas.
 *
 * @pre arq != NULL, formas != NULL.
 *
 * @see svg_desenha_lista_altura
 */
void svg_desenha_lista(FILE* arq, Lista* formas);

/**
 * @brief Desenha todas as formas da lista reutilizando uma altura já calculada.
 *
 * Use esta versão (em vez de svg_desenha_lista) sempre que a altura já
 * tiver sido obtida via svg_calcula_dimensoes, para evitar recalcular
 * a bounding box duas vezes sobre a mesma lista.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param formas Lista de formas a serem desenhadas.
 * @param altura Altura do canvas (já calculada).
 *
 * @pre arq != NULL, formas != NULL, altura > 0.
 */
void svg_desenha_lista_altura(FILE* arq, Lista* formas, double altura);

/* ============================================================
 * FUNÇÕES DE ALTO NÍVEL
 * ============================================================ */

/**
 * @brief Função principal de geração de SVG.
 *
 * Abre o arquivo, calcula dimensões, desenha todas as formas e fecha.
 *
 * @param caminho Caminho do arquivo SVG a ser gerado.
 * @param formas  Lista de formas a serem desenhadas.
 *
 * @return 1 em caso de sucesso, 0 se falhou ao abrir o arquivo.
 *
 * @pre caminho != NULL, formas != NULL.
 * @post Um arquivo SVG é criado no caminho especificado.
 */
int svg_gera_arquivo(char* caminho, Lista* formas);

/**
 * @brief Desenha a região de seleção e marca as âncoras das formas selecionadas.
 *
 * - Região: retângulo com borda vermelha pontilhada.
 * - Âncoras: anel vermelho em volta da âncora de cada forma selecionada.
 *
 * @param arq          Ponteiro para o arquivo aberto.
 * @param x            Coordenada X do canto superior esquerdo da região.
 * @param y            Coordenada Y do canto superior esquerdo da região.
 * @param w            Largura da região.
 * @param h            Altura da região.
 * @param selecionadas Lista de formas selecionadas.
 * @param altura       Altura do canvas (usada para conversão Y).
 *
 * @pre arq != NULL, selecionadas != NULL.
 */
void svg_desenha_selecao(FILE* arq, double x, double y,
                         double w, double h,
                         Lista* selecionadas, double altura);

/**
 * @brief Desenha um "X" vermelho na âncora de uma forma removida.
 *
 * Usado pelo comando 'dels' para marcar visualmente as formas removidas.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param ax     Coordenada X da âncora (sistema cartesiano).
 * @param ay     Coordenada Y da âncora (sistema cartesiano).
 * @param altura Altura do canvas (usada para conversão Y).
 *
 * @pre arq != NULL.
 */
void svg_desenha_x_remocao(FILE* arq, double ax, double ay, double altura);

/**
 * @brief Desenha um pequeno quadrado vermelho na âncora de uma forma selecionada.
 *
 * Conforme o PDF: "marcar a âncora dos k menores com um pequeno quadrado vermelho".
 * Usado pelo comando 'find' no SVG final.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param ax     Coordenada X da âncora (sistema cartesiano).
 * @param ay     Coordenada Y da âncora (sistema cartesiano).
 * @param altura Altura do canvas (usada para conversão Y).
 *
 * @pre arq != NULL.
 */
void svg_desenha_quadrado_selecao(FILE* arq, double ax, double ay, double altura);

/** @} */ /* end of svg group */

#endif /* SVG_H */