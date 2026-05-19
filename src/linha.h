#ifndef LINHA_H
#define LINHA_H

#include "forma.h"

/*
 * Modulo linha — operacoes especificas para FORMA_LINHA.
 * Uma linha e definida por duas extremidades: (x1,y1) e (x2,y2).
 * A ancora armazenada em forma e o ponto (x1,y1).
 * O segundo ponto (x2,y2) e guardado nos dados especificos.
 */

/*
 * linha_cria
 * cria uma forma do tipo linha com extremidades (x1,y1) e (x2,y2)
 * pre-condicao: id >= 1, cor != NULL
 * pos-condicao: retorna Forma criada, NULL se falhar
 */
Forma* linha_cria(int id, double x1, double y1, double x2, double y2,
                  char* cor);

/*
 * linha_get_x2 / linha_get_y2
 * pre-condicao: f != NULL e f eh do tipo FORMA_LINHA
 * pos-condicao: retorna a segunda extremidade
 */
double linha_get_x2(Forma* f);
double linha_get_y2(Forma* f);

/*
 * linha_get_comprimento
 * pos-condicao: retorna o comprimento euclidiano da linha
 */
double linha_get_comprimento(Forma* f);

#endif