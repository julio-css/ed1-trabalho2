#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "arvore.h"
#include "forma.h"

/*
 * geo.h — funcoes de suporte geometrico e leitura do .geo
 *
 * Este modulo e responsavel por:
 *   - calcular propriedades geometricas das formas (area, etc)
 *   - fornecer a funcao de comparacao da ABB (chave: y, x, area)
 *   - ler o arquivo .geo e popular a arvore
 *
 * Fica separado do main.c porque depende de forma.h,
 * e a arvore e generica — ela nao pode incluir forma.h diretamente.
 */

/*
 * geo_area — calcula a area de uma forma conforme as regras do trabalho:
 *   circulo  : pi * r^2
 *   retangulo: w * h
 *   linha    : 1.5 * comprimento
 *   texto    : 10.0 * numero de caracteres
 *   poligono : 0
 */
double geo_area(Forma* f);

/*
 * geo_largura — retorna a largura logica de uma forma:
 *   retangulo: w
 *   circulo  : 2 * r
 *   linha    : comprimento horizontal (|x2 - x1|)
 *   texto    : 1.0 * numero de caracteres
 *   poligono : 0
 */
double geo_largura(Forma* f);

/*
 * geo_altura — retorna a altura logica de uma forma:
 *   retangulo: h
 *   circulo  : 2 * r
 *   linha    : 1.5 (conforme especificacao)
 *   texto    : 10.0 (conforme especificacao)
 *   poligono : 0
 */
double geo_altura(Forma* f);

/*
 * geo_comparar — funcao de comparacao para a ABB.
 * Chave: y crescente, depois x crescente, depois area crescente.
 * Retorna negativo se f1 vem antes, positivo se vem depois.
 * Usa tolerancia de 1e-9 para comparacao de doubles.
 *
 * Assinatura compativel com FuncaoComparacao de arvore.h.
 */
int geo_comparar(void* f1, void* f2);

/*
 * geo_get_id — extrai o ID de uma Forma passada como void*.
 * Usada como callback em buscarPorIdArvore para manter
 * a arvore desacoplada do modulo Forma.
 */
int geo_get_id(void* elemento);

/*
 * geo_processa_arquivo — le o .geo linha por linha e insere
 * cada forma na arvore usando geo_comparar como chave.
 *
 * pre-condicao: arq_geo != NULL, formas != NULL
 * pos-condicao: todas as formas do arquivo estao na arvore
 */
void geo_processa_arquivo(FILE* arq_geo, Arvore formas);

#endif