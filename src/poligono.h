#ifndef POLIGONO_H
#define POLIGONO_H

/*
 * tipo abstrato Poligono
 *
 * um poligono e uma sequencia de pontos (ancora de figuras)
 * armazenados em ordem de insercao numa fila circular estatica.
 * os pontos sao usados para gerar linhas de borda e preenchimento
 * pelo comando pol do arquivo .qry
 *
 * a struct e definida somente em poligono.c
 */
typedef struct Poligono Poligono;

/*
 * pol_cria
 * pos-condicao: retorna poligono vazio, ou NULL se falhar
 */
Poligono* pol_cria();

/*
 * pol_destroi
 * pos-condicao: libera toda memoria do poligono
 */
void pol_destroi(Poligono* p);

/*
 * pol_insere
 * insere coordenada (x,y) da figura id no poligono
 * pre-condicao: p != NULL
 * pos-condicao: retorna 1 se inseriu, 0 se fila cheia
 */
int pol_insere(Poligono* p, double x, double y, int id);

/*
 * pol_remove
 * remove a coordenada mais antiga (FIFO)
 * pre-condicao: p != NULL
 * pos-condicao: preenche ox, oy, oid com os dados removidos
 *               retorna 1 se removeu, 0 se vazio
 */
int pol_remove(Poligono* p, double* ox, double* oy, int* oid);

/*
 * pol_limpa
 * remove todas as coordenadas do poligono
 * pre-condicao: p != NULL
 * pos-condicao: poligono fica vazio
 */
void pol_limpa(Poligono* p);

/*
 * pol_tamanho
 * pos-condicao: retorna quantos pontos tem no poligono
 */
int pol_tamanho(Poligono* p);

/*
 * pol_vazio
 * pos-condicao: retorna 1 se vazio, 0 caso contrario
 */
int pol_vazio(Poligono* p);

/*
 * pol_cheio
 * pos-condicao: retorna 1 se cheio, 0 caso contrario
 */
int pol_cheio(Poligono* p);

/*
 * pol_get
 * acessa ponto pelo indice sem remover (0 = mais antigo)
 * pre-condicao: p != NULL, idx >= 0
 * pos-condicao: preenche ox, oy, oid
 *               retorna 1 se indice valido, 0 caso contrario
 */
int pol_get(Poligono* p, int idx, double* ox, double* oy, int* oid);

#endif