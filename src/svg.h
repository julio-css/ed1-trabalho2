#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "lista.h"
#include "forma.h"

/*
 * Modulo SVG — gera arquivos .svg a partir da lista de formas.
 *
 * O SVG usa coordenadas com y crescendo para BAIXO.
 * O plano cartesiano do trabalho tem y crescendo para CIMA.
 * A conversao e feita internamente: svg_y = altura - cartesiano_y
 *
 * As dimensoes do SVG sao calculadas automaticamente
 * com base nas coordenadas maximas das formas.
 */

/*
 * svg_calcula_dimensoes
 * percorre a lista e calcula largura e altura necessarias
 * adiciona uma margem de 20px em cada lado
 * pre-condicao: formas != NULL
 * pos-condicao: preenche largura e altura
 */
void svg_calcula_dimensoes(Lista* formas, double* largura, double* altura);

/*
 * svg_abre
 * escreve o cabecalho do arquivo SVG
 * pre-condicao: arq != NULL, largura > 0, altura > 0
 */
void svg_abre(FILE* arq, double largura, double altura);

/*
 * svg_fecha
 * escreve o fechamento do arquivo SVG
 * pre-condicao: arq != NULL
 */
void svg_fecha(FILE* arq);

/*
 * svg_desenha_forma
 * desenha uma forma no arquivo SVG fazendo a conversao de coordenadas
 * pre-condicao: arq != NULL, f != NULL, altura > 0
 */
void svg_desenha_forma(FILE* arq, Forma* f, double altura);

/*
 * svg_desenha_lista
 * desenha todas as formas da lista no arquivo SVG
 * pre-condicao: arq != NULL, formas != NULL
 */
void svg_desenha_lista(FILE* arq, Lista* formas);

/*
 * svg_gera_arquivo
 * funcao principal — abre o arquivo, desenha tudo e fecha
 * pre-condicao: caminho != NULL, formas != NULL
 * pos-condicao: retorna 1 se gerou, 0 se falhou ao abrir
 */
int svg_gera_arquivo(char* caminho, Lista* formas);

/*
 * svg_desenha_selecao
 * desenha a regiao de selecao com borda vermelha pontilhada
 * e um anel vermelho em volta da ancora das formas selecionadas
 * pre-condicao: arq != NULL, selecionadas != NULL
 */
void svg_desenha_selecao(FILE* arq, double x, double y,
                         double w, double h,
                         Lista* selecionadas, double altura);

/*
 * svg_desenha_x_remocao
 * desenha um x vermelho na ancora de uma forma removida
 * pre-condicao: arq != NULL, f != NULL
 */
void svg_desenha_x_remocao(FILE* arq, double ax, double ay, double altura);

#endif