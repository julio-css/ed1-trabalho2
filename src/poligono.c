#include <stdlib.h>
#include "poligono.h"
#include "fila.h"

/*
 * ============================================================
 * ESTRUTURAS INTERNAS
 * ============================================================
 */

/**
 * Ponto – estrutura interna que armazena as coordenadas e o ID
 * de uma figura que foi inserida no polígono via comando 'inp'.
 *
 * Os pontos são armazenados em uma fila (FIFO) para manter a ordem
 * de inserção, conforme especificação do trabalho.
 */
typedef struct
{
    double x, y;
    int id;
} Ponto;

/**
 * Poligono – implementação concreta do tipo abstrato Poligono.
 *
 * Utiliza uma fila circular estática para armazenar os pontos.
 * A fila é usada para facilitar a remoção do ponto mais antigo
 * (FIFO) no comando 'rmp'.
 */
struct Poligono
{
    Fila *fila;
};

/*
 * ============================================================
 * CONSTRUTOR E DESTRUIDOR
 * ============================================================
 */

/**
 * pol_cria – cria um novo polígono vazio.
 *
 * @return Ponteiro para o novo polígono, ou NULL em caso de falha.
 */
Poligono *pol_cria()
{
    Poligono *p = (Poligono *)malloc(sizeof(Poligono));
    if (p == NULL)
        return NULL;

    p->fila = fila_cria();
    if (p->fila == NULL)
    {
        free(p);
        return NULL;
    }
    return p;
}

/**
 * pol_destroi – libera toda a memória do polígono.
 *
 * Libera primeiro todos os pontos armazenados na fila,
 * depois a fila e, por fim, a estrutura do polígono.
 */
void pol_destroi(Poligono *p)
{
    if (p == NULL)
        return;

    /* Libera cada ponto que está na fila */
    while (!fila_vazia(p->fila))
    {
        Ponto *pt = (Ponto *)fila_remove(p->fila);
        free(pt);
    }

    fila_destroi(p->fila);
    free(p);
}

/*
 * ============================================================
 * OPERAÇÕES DE INSERÇÃO E REMOÇÃO
 * ============================================================
 */

/**
 * pol_insere – insere uma nova coordenada no polígono.
 *
 * Cria um novo ponto e o insere no final da fila.
 *
 * @return 1 em caso de sucesso, 0 se a fila estiver cheia
 *         ou se falhar a alocação do ponto.
 */
int pol_insere(Poligono *p, double x, double y, int id)
{
    if (fila_cheia(p->fila))
        return 0;

    Ponto *pt = (Ponto *)malloc(sizeof(Ponto));
    if (pt == NULL)
        return 0;

    pt->x = x;
    pt->y = y;
    pt->id = id;

    return fila_insere(p->fila, pt);
}

/**
 * pol_remove – remove o ponto mais antigo do polígono (FIFO).
 *
 * @param ox  Ponteiro para armazenar a coordenada X removida.
 * @param oy  Ponteiro para armazenar a coordenada Y removida.
 * @param oid Ponteiro para armazenar o ID removido.
 *
 * @return 1 em caso de sucesso, 0 se o polígono estiver vazio.
 */
int pol_remove(Poligono *p, double *ox, double *oy, int *oid)
{
    Ponto *pt = (Ponto *)fila_remove(p->fila);
    if (pt == NULL)
        return 0;

    *ox = pt->x;
    *oy = pt->y;
    *oid = pt->id;
    free(pt);
    return 1;
}

/**
 * pol_limpa – remove todos os pontos do polígono.
 *
 * Esvazia a fila e libera a memória de cada ponto.
 */
void pol_limpa(Poligono *p)
{
    while (!fila_vazia(p->fila))
    {
        Ponto *pt = (Ponto *)fila_remove(p->fila);
        free(pt);
    }
}

/*
 * ============================================================
 * CONSULTAS DE ESTADO
 * ============================================================
 */

int pol_tamanho(Poligono *p) { return fila_tamanho(p->fila); }
int pol_vazio(Poligono *p) { return fila_vazia(p->fila); }
int pol_cheio(Poligono *p) { return fila_cheia(p->fila); }

/*
 * ============================================================
 * ACESSO POR ÍNDICE
 * ============================================================
 */

/**
 * pol_get – acessa um ponto pelo índice sem removê-lo.
 *
 * @param idx Índice do ponto (0 = mais antigo).
 * @param ox  Ponteiro para armazenar a coordenada X.
 * @param oy  Ponteiro para armazenar a coordenada Y.
 * @param oid Ponteiro para armazenar o ID.
 *
 * @return 1 se o índice é válido, 0 caso contrário.
 */
int pol_get(Poligono *p, int idx, double *ox, double *oy, int *oid)
{
    Ponto *pt = (Ponto *)fila_get(p->fila, idx);
    if (pt == NULL)
        return 0;

    *ox = pt->x;
    *oy = pt->y;
    *oid = pt->id;
    return 1;
}