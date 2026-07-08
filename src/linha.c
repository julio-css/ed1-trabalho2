#include <math.h>
#include "linha.h"
#include "forma.h"

/*
 * ============================================================
 * CONSTRUTOR
 * ============================================================
 */

/**
 * linha_cria – delega a criação para forma_cria_linha.
 *
 * Note que a função base (forma_cria_linha) automaticamente duplica o
 * parâmetro 'cor' para preencher tanto cor_borda quanto cor_preenchimento,
 * já que linhas no padrão SVG têm apenas o atributo 'stroke' (borda).
 * O preenchimento não se aplica a linhas.
 */
Forma *linha_cria(int id, double x1, double y1, double x2, double y2, char *cor)
{
  return forma_cria_linha(id, x1, y1, x2, y2, cor);
}

/*
 * ============================================================
 * GETTERS
 * ============================================================
 */

/**
 * linha_get_x2 – obtém a coordenada X da segunda extremidade.
 *
 * O acesso à segunda extremidade faz uso direto da união (union)
 * presente no arquivo forma.c, através do getter genérico.
 * A primeira extremidade é armazenada em (x, y) da Forma.
 */
double linha_get_x2(Forma *f)
{
  return forma_get_x2(f);
}

/**
 * linha_get_y2 – obtém a coordenada Y da segunda extremidade.
 */
double linha_get_y2(Forma *f)
{
  return forma_get_y2(f);
}

/*
 * ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================
 */

/**
 * linha_get_comprimento – calcula o comprimento euclidiano da linha.
 *
 * Utiliza o Teorema de Pitágoras adaptado para a distância euclidiana
 * em um plano 2D: C = √((x2 - x1)² + (y2 - y1)²)
 *
 * Os pontos (x1, y1) são recuperados da âncora geral da Forma,
 * enquanto (x2, y2) são recuperados da struct específica DadosLinha.
 */
double linha_get_comprimento(Forma *f)
{
  double dx = forma_get_x2(f) - forma_get_x(f);
  double dy = forma_get_y2(f) - forma_get_y(f);
  return sqrt(dx * dx + dy * dy);
}