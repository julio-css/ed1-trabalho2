#include <stdlib.h>
#include "poligono.h"
#include "fila.h"

/* ponto interno — armazena coordenadas e id da figura */
typedef struct {
    double x, y;
    int id;
} Ponto;

/* agora o poligono usa a Fila separada */
struct Poligono {
    Fila* fila;
};

Poligono* pol_cria() {
    Poligono* p = (Poligono*) malloc(sizeof(Poligono));
    if (p == NULL) return NULL;
    p->fila = fila_cria();
    if (p->fila == NULL) {
        free(p);
        return NULL;
    }
    return p;
}

void pol_destroi(Poligono* p) {
    if (p == NULL) return;
    /* libera cada ponto que esta na fila */
    while (!fila_vazia(p->fila)) {
        Ponto* pt = (Ponto*) fila_remove(p->fila);
        free(pt);
    }
    fila_destroi(p->fila);
    free(p);
}

int pol_insere(Poligono* p, double x, double y, int id) {
    if (fila_cheia(p->fila)) return 0;
    Ponto* pt = (Ponto*) malloc(sizeof(Ponto));
    if (pt == NULL) return 0;
    pt->x  = x;
    pt->y  = y;
    pt->id = id;
    return fila_insere(p->fila, pt);
}

int pol_remove(Poligono* p, double* ox, double* oy, int* oid) {
    Ponto* pt = (Ponto*) fila_remove(p->fila);
    if (pt == NULL) return 0;
    *ox  = pt->x;
    *oy  = pt->y;
    *oid = pt->id;
    free(pt);
    return 1;
} 

void pol_limpa(Poligono* p) {
    while (!fila_vazia(p->fila)) {
        Ponto* pt = (Ponto*) fila_remove(p->fila);
        free(pt);
    }
}

int pol_tamanho(Poligono* p) { return fila_tamanho(p->fila); }
int pol_vazio(Poligono* p)   { return fila_vazia(p->fila); }
int pol_cheio(Poligono* p)   { return fila_cheia(p->fila); }

int pol_get(Poligono* p, int idx, double* ox, double* oy, int* oid) {
    Ponto* pt = (Ponto*) fila_get(p->fila, idx);
    if (pt == NULL) return 0;
    *ox  = pt->x;
    *oy  = pt->y;
    *oid = pt->id;
    return 1;
} 