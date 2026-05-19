#ifndef FORMA_H
#define FORMA_H

#define MAX_COR   32
#define MAX_TEXTO 256

/*
 * Modulo Forma (Base Polimorfica)
 * -------------------------------
 * 'Forma' eh o tipo base generico que representa qualquer figura geom.
 * A struct interna eh "opaca" (definida somente em forma.c) para garantir 
   um encapsulamento estrito. O acesso aos dados eh feito EXCLUSIVAMENTE
   pelas funcoes (getters/setters).
 */
typedef struct Forma Forma;

/*
 * TipoForma atua como uma "etiqueta" para o polimorfismo.
 * Ele indica qual figura geometrica a Forma generica esta a guardar no momento,
   determinando quais campos especificos (raio, largura, texto) sao validos.
 */
typedef enum {
    FORMA_CIRCULO,
    FORMA_RETANGULO,
    FORMA_LINHA,
    FORMA_TEXTO,
    FORMA_POLIGONO 
} TipoForma;

/* --- CONSTRUTORES --- */

Forma* forma_cria_circulo(int id, double x, double y, double r, char* cor_borda, char* cor_preench);
Forma* forma_cria_retangulo(int id, double x, double y, double w, double h, char* cor_borda, char* cor_preench);
Forma* forma_cria_linha(int id, double x1, double y1, double x2, double y2, char* cor);
Forma* forma_cria_texto(int id, double x, double y, char* cor_borda, char* cor_preench, char ancora_tipo, char* texto);

/* NOVO CONSTRUTOR: Cria um polígono recebendo um array de coordenadas X e Y */
Forma* forma_cria_poligono(int id, int num_pontos, double* pts_x, double* pts_y, char* cor_borda, char* cor_preench);

/* --- DESTRUIDOR --- */
void forma_destroi(Forma* f);

/* --- GETTERS (Leitura de Dados) --- */
int forma_get_id(Forma* f);
TipoForma forma_get_tipo(Forma* f);

double forma_get_x(Forma* f);
double forma_get_y(Forma* f);
double forma_get_x2(Forma* f);
double forma_get_y2(Forma* f);
double forma_get_raio(Forma* f);
double forma_get_largura(Forma* f);
double forma_get_altura(Forma* f);
char* forma_get_cor_borda(Forma* f);
char* forma_get_cor_preench(Forma* f);
char* forma_get_texto(Forma* f);
char forma_get_ancora(Forma* f);

/* GETTERS DO POLÍGONO */
int forma_get_num_pontos(Forma* f);
double forma_get_ponto_x(Forma* f, int i);
double forma_get_ponto_y(Forma* f, int i);

/* --- SETTERS (Alteracao de Dados) --- */
void forma_set_x(Forma* f, double x);
void forma_set_y(Forma* f, double y);
void forma_set_x2(Forma* f, double x2);
void forma_set_y2(Forma* f, double y2);
void forma_set_cor_borda(Forma* f, char* cor);
void forma_set_cor_preench(Forma* f, char* cor);

/* SETTERS DO POLÍGONO (Útil para o comando mcs mover os vértices) */
void forma_set_ponto_x(Forma* f, int i, double x);
void forma_set_ponto_y(Forma* f, int i, double y);

#endif