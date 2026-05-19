#ifndef RETANGULO_H
#define RETANGULO_H

#include "forma.h"


/*
 * Modulo retangulo — operacoes especificas para FORMA_RETANGULO.
 * A ancora do retangulo e o canto inferior esquerdo (x,y).
 * O retangulo se extende de (x,y) ate (x+w, y+h).
 */


 
/*
 * retangulo_cria
 * cria uma forma do tipo retangulo
 * pre-condicao: id >= 1, w > 0, h > 0
 * pos-condicao: retorna Forma criada, NULL se falhar
 */
Forma* retangulo_cria(int id, double x, double y, double w, double h,
                      char* cor_borda, char* cor_preench);

/*
 * retangulo_get_largura / retangulo_get_altura
 * pre-condicao: f != NULL e f eh do tipo FORMA_RETANGULO
 * pos-condicao: retorna largura ou altura
 */
double retangulo_get_largura(Forma* f);
double retangulo_get_altura(Forma* f);

/*
 * retangulo_get_area
 * pos-condicao: retorna a area (w * h)
 */
double retangulo_get_area(Forma* f);

/*
 * retangulo_contem_ponto
 * verifica se ponto (px,py) esta dentro do retangulo
 * pre-condicao: f != NULL e f eh do tipo FORMA_RETANGULO
 * pos-condicao: retorna 1 se contem, 0 caso contrario
 */
int retangulo_contem_ponto(Forma* f, double px, double py);

#endif