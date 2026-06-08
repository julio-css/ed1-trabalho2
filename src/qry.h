#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include "arvore.h"
#include "lista.h"
#include "poligono.h"

/*
 * qry.h — processamento do arquivo .qry e helpers de SVG com arvore
 *
 * Este modulo executa os comandos do .qry sobre a arvore de formas.
 * Tambem contem os wrappers que conectam a Arvore ao modulo SVG,
 * mantendo o svg.h desacoplado da estrutura de dados.
 *
 * Comandos tratados:
 *   inp  — insere ancora de uma figura no poligono
 *   rmp  — remove ancora mais antiga do poligono
 *   clp  — limpa todas as coordenadas do poligono
 *   pol  — gera as linhas do poligono e insere na arvore
 *   sel  — seleciona figuras dentro de uma regiao
 *   dels — remove as figuras selecionadas
 *   mcs  — move e recolore as figuras selecionadas
 */

/* numero maximo de poligonos simultaneos conforme o trabalho */
#define MAX_POLIGONOS 10

/*
 * qry_svg_de_arvore — gera o SVG com todas as formas da arvore em ordem.
 * Monta uma lista temporaria internamente e chama svg_gera_arquivo.
 * A lista e destruida ao final — as formas nao sao liberadas.
 *
 * pre-condicao: caminho != NULL, formas != NULL
 */
void qry_svg_de_arvore(const char* caminho, Arvore formas);

/*
 * qry_processa_arquivo — le o .qry e executa cada comando sobre a arvore.
 *
 * base_svg: caminho base para SVGs intermediarios gerados por sel e dels.
 *           Passa NULL se nao quiser gerar SVGs intermediarios.
 *
 * pre-condicao: arq_qry != NULL, formas != NULL, poligonos != NULL
 * pos-condicao: arvore e poligonos refletem o estado apos todos os comandos
 */
void qry_processa_arquivo(FILE* arq_qry, Arvore formas,
                          Poligono** poligonos, FILE* arq_txt,
                          const char* base_svg);

#endif