#include <math.h>
#include "linha.h"
#include "forma.h"

/* A criacao eh totalmente delegada a base generica. 
   Note que a funcao base (forma_cria_linha) automaticamente duplica o
   parametro 'cor' para preencher tanto cor_borda quanto cor_preenchimento,
   ja que linhas no padrao SVG tem apenas o atributo 'stroke' .
 */
Forma* linha_cria(int id, double x1, double y1, double x2, double y2, char* cor) {
    return forma_cria_linha(id, x1, y1, x2, y2, cor);
}

/* O acesso a segunda extremidade faz uso direto da uniao (union) 
   presente no arquivo forma.c.
 */
double linha_get_x2(Forma* f) { return forma_get_x2(f); }
double linha_get_y2(Forma* f) { return forma_get_y2(f); }

/*
 * Calcula o comprimento da reta.
 * Utilizei o Teorema de Pitgoras adaptado para a
   distancia eclidiana em um plano 2D. 
   Formula: C = √((x2 - x1)² + (y2 - y1)²)
 * Os pontos x1 e y1 sao recuperados da ancora geral da Forma, enquanto
   x2 e y2 sao recuperados da struct especufica (DadosLinha).
 */
double linha_get_comprimento(Forma* f) {
    double dx = forma_get_x2(f) - forma_get_x(f);
    double dy = forma_get_y2(f) - forma_get_y(f);
    return sqrt(dx*dx + dy*dy);
}