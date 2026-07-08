#include "retangulo.h"
#include "forma.h"

/*
 * ============================================================
 * CONSTRUTOR
 * ============================================================
 */

/**
 * retangulo_cria – delega a criação para forma_cria_retangulo.
 *
 * A criação efetiva e a alocação de memória são delegadas à função
 * genérica forma_cria_retangulo, centralizando a lógica de criação
 * no módulo forma.c.
 */
Forma *retangulo_cria(int id, double x, double y, double w, double h,
                      char *cor_borda, char *cor_preench)
{
    return forma_cria_retangulo(id, x, y, w, h, cor_borda, cor_preench);
}

/*
 * ============================================================
 * GETTERS
 * ============================================================
 */

/**
 * retangulo_get_largura – obtém a largura do retângulo.
 *
 * O acesso aos dados específicos é feito através dos getters
 * da struct genérica Forma, mantendo o encapsulamento.
 */
double retangulo_get_largura(Forma *f)
{
    return forma_get_largura(f);
}

/**
 * retangulo_get_altura – obtém a altura do retângulo.
 */
double retangulo_get_altura(Forma *f)
{
    return forma_get_altura(f);
}

/*
 * ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================
 */

/**
 * retangulo_get_area – calcula a área do retângulo.
 *
 * Área = largura * altura.
 * A âncora do retângulo é o canto inferior esquerdo (x, y).
 */
double retangulo_get_area(Forma *f)
{
    return forma_get_largura(f) * forma_get_altura(f);
}

/**
 * retangulo_contem_ponto – verifica se um ponto está dentro do retângulo.
 *
 * A âncora é o canto inferior esquerdo (x, y).
 * O retângulo ocupa a região de x até x+w e de y até y+h.
 *
 * @return 1 (verdadeiro) se o ponto está dentro, 0 (falso) caso contrário.
 */
int retangulo_contem_ponto(Forma *f, double px, double py)
{
    double x = forma_get_x(f);
    double y = forma_get_y(f);
    double w = forma_get_largura(f);
    double h = forma_get_altura(f);
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}