#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qry.h"
#include "geo.h"
#include "arvore.h"
#include "lista.h"
#include "forma.h"
#include "poligono.h"
#include "svg.h"

/* =============================================================
 * HELPERS INTERNOS — SVG COM ARVORE
 * O modulo SVG so conhece Lista*. Esses helpers fazem a ponte
 * entre a Arvore e o SVG sem acoplar os dois modulos.
 * ============================================================= */

/*
 * cb_coleta_lista — callback para emOrdemArvore.
 * Insere cada forma visitada na lista auxiliar.
 */
static void cb_coleta_lista(void* elemento, void* aux) {
    lista_inserir_fim((Lista*) aux, elemento);
}

/*
 * qry_svg_de_arvore — gera o SVG com todas as formas em ordem.
 * Monta lista temporaria, chama svg_gera_arquivo, destroi a lista.
 */
void qry_svg_de_arvore(const char* caminho, Arvore formas) {
    Lista* temp = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, temp);
    svg_gera_arquivo((char*) caminho, temp);
    lista_destruir(temp);
}

/*
 * CtxSvgSel — contexto para gerar o SVG do sel.
 * Agrupa a lista de selecionadas e os parametros da regiao.
 */
typedef struct {
    Lista* selecionadas;
    double sel_x, sel_y, sel_w, sel_h;
} CtxSvgSel;

/*
 * svg_com_selecao — gera SVG com todas as formas mais:
 *   - retangulo pontilhado vermelho da regiao sel
 *   - anel vermelho na ancora de cada forma selecionada
 */
static void svg_com_selecao(const char* caminho, Arvore formas,
                             CtxSvgSel* ctx) {
    Lista* todas = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, todas);

    FILE* arq = fopen(caminho, "w");
    if (!arq) { lista_destruir(todas); return; }

    double l, a;
    svg_calcula_dimensoes(todas, &l, &a);
    svg_abre(arq, l, a);
    svg_desenha_lista(arq, todas);
    svg_desenha_selecao(arq,
                        ctx->sel_x, ctx->sel_y,
                        ctx->sel_w, ctx->sel_h,
                        ctx->selecionadas, a);
    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/*
 * svg_com_remocao — gera SVG com todas as formas mais um X vermelho
 * na ancora de cada forma que sera removida pelo dels.
 * Chamado antes de remover, para registrar visualmente onde estavam.
 */
static void svg_com_remocao(const char* caminho, Arvore formas,
                             Lista* selecionadas) {
    Lista* todas = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, todas);

    FILE* arq = fopen(caminho, "w");
    if (!arq) { lista_destruir(todas); return; }

    double l, a;
    svg_calcula_dimensoes(todas, &l, &a);
    svg_abre(arq, l, a);
    svg_desenha_lista(arq, todas);

    int n = lista_tamanho(selecionadas);
    for (int i = 0; i < n; i++) {
        Forma* f = (Forma*) lista_get(selecionadas, i);
        svg_desenha_x_remocao(arq, forma_get_x(f), forma_get_y(f), a);
    }

    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}


/* =============================================================
 * PREDICADO DE SELECAO
 * ============================================================= */

/*
 * RegiaoSel — retangulo de selecao passado como contexto ao predicado.
 */
typedef struct {
    double x, y, w, h;
} RegiaoSel;

/*
 * predicado_na_regiao — retorna 1 se a ancora da forma esta
 * dentro do retangulo da RegiaoSel.
 * Para poligonos, basta um vertice estar dentro.
 */
static int predicado_na_regiao(void* elemento, void* ctx) {
    Forma*     f = (Forma*) elemento;
    RegiaoSel* r = (RegiaoSel*) ctx;

    if (forma_get_tipo(f) == FORMA_POLIGONO) {
        int n = forma_get_num_pontos(f);
        for (int k = 0; k < n; k++) {
            double px = forma_get_ponto_x(f, k);
            double py = forma_get_ponto_y(f, k);
            if (px >= r->x && px <= r->x + r->w &&
                py >= r->y && py <= r->y + r->h)
                return 1;
        }
        return 0;
    }

    double fx = forma_get_x(f);
    double fy = forma_get_y(f);
    return (fx >= r->x && fx <= r->x + r->w &&
            fy >= r->y && fy <= r->y + r->h);
}

static void cb_coleta_selecionadas(void* elemento, void* aux) {
    lista_inserir_fim((Lista*) aux, elemento);
}


/* =============================================================
 * PROCESSAMENTO DO .QRY
 * ============================================================= */

/*
 * qry_processa_arquivo — le o .qry e executa cada comando
 */
void qry_processa_arquivo(FILE* arq_qry, Arvore formas,
                          Poligono** poligonos, FILE* arq_txt,
                          const char* base_svg) {
    char cmd[8];
    Lista* selecionadas = lista_criar();

    while (fscanf(arq_qry, "%7s", cmd) == 1) {

        if (strcmp(cmd, "inp") == 0) {
            /* inp p i — insere ancora da figura i no poligono p */
            int p, i;
            fscanf(arq_qry, "%d %d", &p, &i);
            if (p < 1 || p > MAX_POLIGONOS) continue;

            Forma* f = (Forma*) buscarPorIdArvore(formas, i, geo_get_id);
            if (f == NULL) continue;

            double ax = forma_get_x(f), ay = forma_get_y(f);
            /* linha: usa a extremidade de menor x como ancora */
            if (forma_get_tipo(f) == FORMA_LINHA) {
                double x2 = forma_get_x2(f), y2 = forma_get_y2(f);
                if (x2 < ax || (x2 == ax && y2 < ay)) { ax = x2; ay = y2; }
            }
            pol_insere(poligonos[p-1], ax, ay, i);
            if (arq_txt)
                fprintf(arq_txt, "[*] inp %d %d -> (%.2f, %.2f)\n", p, i, ax, ay);

        } else if (strcmp(cmd, "rmp") == 0) {
            /* rmp p — remove a ancora mais antiga do poligono p */
            int p; fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS) continue;
            double ox, oy; int oid;
            if (pol_remove(poligonos[p-1], &ox, &oy, &oid))
                if (arq_txt)
                    fprintf(arq_txt, "[*] rmp %d: removido pt de id %d\n", p, oid);

        } else if (strcmp(cmd, "clp") == 0) {
            /* clp p — limpa todas as coordenadas do poligono p */
            int p; fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS) continue;
            pol_limpa(poligonos[p-1]);
            if (arq_txt)
                fprintf(arq_txt, "[*] clp %d: poligono esvaziado\n", p);

        } else if (strcmp(cmd, "pol") == 0) {
            /* pol p id esp corb corp — gera o poligono p como forma e insere */
            int p, id_pol; double esp; char corb[32], corp[32];
            fscanf(arq_qry, "%d %d %lf %31s %31s", &p, &id_pol, &esp, corb, corp);
            if (p >= 1 && p <= MAX_POLIGONOS) {
                int n_pts = pol_tamanho(poligonos[p-1]);
                if (n_pts >= 3) {
                    double* px = (double*) malloc(n_pts * sizeof(double));
                    double* py = (double*) malloc(n_pts * sizeof(double));
                    int oid;
                    for (int k = 0; k < n_pts; k++)
                        pol_get(poligonos[p-1], k, &px[k], &py[k], &oid);
                    Forma* f = forma_cria_poligono(id_pol, n_pts, px, py, corb, corp);
                    if (f) inserirArvore(formas, f);
                    free(px); free(py);
                }
            }
            if (arq_txt)
                fprintf(arq_txt, "[*] pol %d: gerado com id %d\n", p, id_pol);

        } else if (strcmp(cmd, "sel") == 0) {
            /* sel x y w h — seleciona formas cuja ancora esta dentro da regiao
             * TXT: reporta id e tipo de cada selecionada
             * SVG: regiao pontilhada vermelha + anel nas ancoras */
            RegiaoSel reg;
            fscanf(arq_qry, "%lf %lf %lf %lf", &reg.x, &reg.y, &reg.w, &reg.h);

            lista_destruir(selecionadas);
            selecionadas = lista_criar();

            emOrdemFiltroArvore(formas,
                                predicado_na_regiao,    &reg,
                                cb_coleta_selecionadas, selecionadas);

            if (arq_txt) {
                fprintf(arq_txt, "[*] sel %.1f %.1f %.1f %.1f\n",
                        reg.x, reg.y, reg.w, reg.h);
                int n = lista_tamanho(selecionadas);
                for (int i = 0; i < n; i++) {
                    Forma* f = (Forma*) lista_get(selecionadas, i);
                    fprintf(arq_txt, "   - id %d tipo %d\n",
                            forma_get_id(f), (int) forma_get_tipo(f));
                }
            }

            if (base_svg != NULL) {
                CtxSvgSel ctx_svg = {
                    selecionadas,
                    reg.x, reg.y, reg.w, reg.h
                };
                svg_com_selecao(base_svg, formas, &ctx_svg);
            }

        } else if (strcmp(cmd, "dels") == 0) {
            /* dels — remove da arvore todas as formas selecionadas
             * SVG: marca com X vermelho antes de remover */
            int n = lista_tamanho(selecionadas);

            if (base_svg != NULL && n > 0)
                svg_com_remocao(base_svg, formas, selecionadas);

            for (int i = 0; i < n; i++) {
                Forma* f = (Forma*) lista_get(selecionadas, i);
                if (arq_txt)
                    fprintf(arq_txt, "[*] dels: removido id %d\n", forma_get_id(f));
                removerArvore(formas, f);
                forma_destroi(f);
            }
            lista_destruir(selecionadas);
            selecionadas = lista_criar();

        } else if (strcmp(cmd, "mcs") == 0) {
            /* mcs dx dy corb corp — translada e recolore as selecionadas
             *
             * ao mover uma forma a chave na ABB muda (y, x, area)
             * entao o procedimento e: remover → atualizar → reinserir */
            double dx, dy; char corb[32], corp[32];
            fscanf(arq_qry, "%lf %lf %31s %31s", &dx, &dy, corb, corp);

            int n = lista_tamanho(selecionadas);
            for (int i = 0; i < n; i++) {
                Forma* f = (Forma*) lista_get(selecionadas, i);

                removerArvore(formas, f);

                forma_set_x(f, forma_get_x(f) + dx);
                forma_set_y(f, forma_get_y(f) + dy);

                if (forma_get_tipo(f) == FORMA_LINHA) {
                    forma_set_x2(f, forma_get_x2(f) + dx);
                    forma_set_y2(f, forma_get_y2(f) + dy);
                } else if (forma_get_tipo(f) == FORMA_POLIGONO) {
                    int n_pts = forma_get_num_pontos(f);
                    for (int j = 0; j < n_pts; j++) {
                        forma_set_ponto_x(f, j, forma_get_ponto_x(f, j) + dx);
                        forma_set_ponto_y(f, j, forma_get_ponto_y(f, j) + dy);
                    }
                }

                forma_set_cor_borda(f, corb);
                forma_set_cor_preench(f, corp);

                inserirArvore(formas, f);
            }
            if (arq_txt)
                fprintf(arq_txt, "[*] mcs: transladadas %d formas\n", n);
        }
    }
    lista_destruir(selecionadas);
}