#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include "arvore.h"
#include "lista.h"
#include "poligono.h"

/**
 * @defgroup qry Qry
 * @brief Módulo de processamento do arquivo .qry e helpers de SVG com árvore.
 *
 * Este módulo executa os comandos do .qry sobre a árvore de formas.
 * Também contém os wrappers que conectam a Arvore ao módulo SVG,
 * mantendo o svg.h desacoplado da estrutura de dados.
 *
 * @see geo.h, svg.h, sort.h
 * @{
 */

/* ============================================================
 * CONSTANTES
 * ============================================================ */

/** Número máximo de polígonos simultâneos conforme o trabalho ([1,10]). */
#define MAX_POLIGONOS 10

/* ============================================================
 * COMANDOS SUPORTADOS
 * ============================================================ */

/**
 * @brief Comandos tratados pelo módulo qry.
 *
 * - inp    : insere âncora de uma figura no polígono
 * - rmp    : remove âncora mais antiga do polígono
 * - clp    : limpa todas as coordenadas do polígono
 * - pol    : gera as linhas do polígono e insere na árvore
 * - sel    : seleciona figuras dentro de uma região (descarta seleção anterior)
 * - dels   : remove as figuras selecionadas
 * - mcs    : move e recolore as figuras selecionadas
 * - mc     : recolore as figuras selecionadas (sem mover)
 * - cm     : clona figuras dentro de uma região, move os clones e os torna seleção atual
 * - find   : ordena figuras selecionadas e posiciona os k menores
 * - findrm : similar ao find, remove figuras de rank superior a k
 */

/* ============================================================
 * FUNÇÕES PÚBLICAS
 * ============================================================ */

/**
 * @brief Gera o SVG com todas as formas da árvore em ordem.
 *
 * Monta uma lista temporária internamente e chama svg_gera_arquivo.
 * A lista é destruída ao final — as formas não são liberadas.
 *
 * @param caminho Caminho do arquivo SVG a ser gerado.
 * @param formas  Ponteiro para a árvore de formas.
 *
 * @pre caminho != NULL, formas != NULL.
 * @post Um arquivo SVG é gerado no caminho especificado.
 */
void qry_svg_de_arvore(const char* caminho, Arvore formas);

/**
 * @brief Lê o arquivo .qry e executa cada comando sobre a árvore.
 *
 * @param arq_qry   Ponteiro para o arquivo .qry aberto para leitura.
 * @param formas    Ponteiro para a árvore de formas.
 * @param poligonos Array de ponteiros para polígonos (tamanho MAX_POLIGONOS).
 * @param arq_txt   Ponteiro para o arquivo de saída de texto (.txt).
 * @param base_svg  Caminho base para SVGs intermediários gerados por sel e dels.
 *                  Passe NULL se não quiser gerar SVGs intermediários.
 *
 * @pre arq_qry != NULL, formas != NULL, poligonos != NULL.
 * @post Árvore e polígonos refletem o estado após todos os comandos.
 *
 * @note O arquivo de texto (arq_txt) pode ser NULL se não houver saída.
 * @see svg_de_arvore_com_sel, svg_de_arvore_com_x, svg_de_arvore_com_quadrados
 */
void qry_processa_arquivo(FILE* arq_qry, Arvore formas,
                          Poligono** poligonos, FILE* arq_txt,
                          const char* base_svg);

/** @} */ /* end of qry group */

#endif /* QRY_H */