#include "retangulo.h"
#include "forma.h"

/* delega criacao para forma.c */
Forma* retangulo_cria(int id, double x, double y, double w, double h,
                      char* cor_borda, char* cor_preench) {
    return forma_cria_retangulo(id, x, y, w, h, cor_borda, cor_preench);
}

/* acessa largura e altura via getters de forma */
double retangulo_get_largura(Forma* f) { return forma_get_largura(f); }
double retangulo_get_altura(Forma* f)  { return forma_get_altura(f); }

/* area = largura * altura */
double retangulo_get_area(Forma* f) {
    return forma_get_largura(f) * forma_get_altura(f);
}

/*
 * verifica se (px,py) esta dentro do retangulo
 * ancora e o canto inferior esquerdo (x,y)
 * retangulo ocupa de x ate x+w e de y ate y+h
 */
int retangulo_contem_ponto(Forma* f, double px, double py) {
    double x = forma_get_x(f);
    double y = forma_get_y(f);
    double w = forma_get_largura(f);
    double h = forma_get_altura(f);
    return (px >= x && px <= x + w && py >= y && py <= y + h);
}
