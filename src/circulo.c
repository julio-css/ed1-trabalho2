#include <math.h>
#include "circulo.h"
#include "forma.h"

/*
 * ============================================================
 * CONSTRUTOR
 * ============================================================
 */

/**
 * circulo_cria – delega a criação para forma_cria_circulo.
 *
 * A criação efetiva e a alocação de memória são delegadas à função
 * genérica forma_cria_circulo. Isso garante que o preenchimento da
 * struct principal (id, coordenadas âncora, cores) permaneça centralizado
 * em forma.c, respeitando o princípio de encapsulamento.
 *
 * O módulo circulo apenas fornece a interface específica para círculos,
 * mas a implementação concreta fica no módulo forma.c, que gerencia
 * a união de dados específicos de cada tipo.
 */
Forma *circulo_cria(int id, double x, double y, double r,
                    char *cor_borda, char *cor_preench)
{
  return forma_cria_circulo(id, x, y, r, cor_borda, cor_preench);
}

/*
 * ============================================================
 * GETTERS
 * ============================================================
 */

/**
 * circulo_get_raio – obtém o raio do círculo.
 *
 * O acesso aos dados internos específicos (raio) é feito unicamente
 * através dos getters da struct genérica Forma, escondendo do utilizador
 * a implementação da união (union) contida em forma.c.
 *
 * Isso mantém o encapsulamento e permite que a struct Forma mude
 * sem afetar os módulos que a utilizam.
 */
double circulo_get_raio(Forma *f)
{
  return forma_get_raio(f);
}

/*
 * ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================
 */

/**
 * circulo_get_area – calcula a área do círculo.
 *
 * Usa a constante Pi com alta precisão para minimizar erros
 * de arredondamento em cálculos sucessivos.
 */
double circulo_get_area(Forma *f)
{
  double r = forma_get_raio(f);
  return 3.141592653589793 * r * r;
}

/**
 * circulo_contem_ponto – testa se um ponto está dentro do círculo.
 *
 * Usa a equação da circunferência: (x-cx)² + (y-cy)² <= r².
 *
 * Otimização: em vez de calcular a distância real usando a raiz
 * quadrada (sqrt) — que é uma operação computacionalmente custosa —,
 * calculamos a distância ao quadrado e comparamos com o raio ao quadrado
 * (dx² + dy² <= r²). O resultado lógico é matematicamente idêntico
 * e poupa ciclos de processador, especialmente importante em operações
 * de seleção (sel) que testam muitos pontos contra muitas formas.
 */
int circulo_contem_ponto(Forma *f, double px, double py)
{
  double cx = forma_get_x(f);
  double cy = forma_get_y(f);
  double r = forma_get_raio(f);

  double dx = px - cx;
  double dy = py - cy;

  return (dx * dx + dy * dy) <= (r * r);
}