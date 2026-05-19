#ifndef TEXTO_H
#define TEXTO_H

#include "forma.h"

/*
 * Modulo texto — operacoes especificas para FORMA_TEXTO.
 * Um texto e posicionado em (x,y) com ancora definida por ancora_tipo.
 * ancora_tipo 'i' = inicio do texto em (x,y)
 * ancora_tipo 'm' = meio do texto em (x,y)
 * ancora_tipo 'f' = fim do texto em (x,y)
 */

 

/*
 * texto_cria
 * cria uma forma do tipo texto
 * ancora_tipo: 'i' inicio, 'm' meio, 'f' fim
 * pre-condicao: id >= 1, conteudo != NULL
 * pos-condicao: retorna Forma criada, NULL se falhar
 */
Forma* texto_cria(int id, double x, double y,
                  char* cor_borda, char* cor_preench,
                  char ancora_tipo, char* conteudo);

#endif
