#ifndef CIRCULO_H
#define CIRCULO_H

#include "forma.h"

/*
 * Módulo Circulo

 * operações especificas para forma geom. do tipo FORMA_CIRCULO.
 * utiliza internamente a estrutura genérica 'Forma' como base.
 * todas as funções deste módulo assumem que o ponteiro 'f'
   passado como parâmetro seja estritamente do tipo FORMA_CIRCULO.
 */

/*
 * cria uma nova instância de um círculo geométrico.
 * pre-condicao: id >= 1, r > 0. os ponteiros cor_borda e cor_preench 
   não podem ser NULL e devem ser strings válidas.
 * pos-condicao: retorna um ponteiro para a nova Forma criada, 
   ou NULL em caso de falha de alocação de memória.
 */
Forma* circulo_cria(int id, double x, double y, double r,
                    char* cor_borda, char* cor_preench);

/*
 * obtem o valor do raio do circulo.
 * pre-condicao: f != NULL e f é do tipo FORMA_CIRCULO.
 * pos-condicao: Retorna o valor do raio (double).
 */ 
double circulo_get_raio(Forma* f);

/*
 * calcula a área ocupada pelo círculo.
 * pre-condicao: f != NULL e f é do tipo FORMA_CIRCULO.
 * pos-condicao: Retorna a área calculada matematicamente (pi * r^2).
 */
double circulo_get_area(Forma* f);

/*
 * verifica se uma coordenada (px, py) ta contida na area do círculo.
 * eh util para operacoes de seleção espacial (como o comando 'sel').
 * pre-condicao: f != NULL e f é do tipo FORMA_CIRCULO.
 * pre-condicao: Retorna 1 (Verdadeiro) se o ponto estiver dentro ou
   exatamente na borda do círculo, ou 0 (Falso) caso contrário.
 */
int circulo_contem_ponto(Forma* f, double px, double py);

#endif