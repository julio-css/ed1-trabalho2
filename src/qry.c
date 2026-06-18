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
#include "sort.h"

#define MAX_PATH 512

/* =============================================================
 * CONTEXTO DOS FRAMES DE ANIMACAO
 * Passado como ctx para o callback de snapshot do sort.
 * ============================================================= */

/*
 * CtxFrames — contexto para geracao dos frames SVG da animacao.
 * base    : caminho base sem extensao (ex: "saida/arq-qry")
 * contador: proximo numero de frame, comeca em 1
 * formas  : arvore com todas as formas (fundo de cada frame)
 */
typedef struct
{
    char base[MAX_PATH * 4];
    int contador;
    Arvore formas;
} CtxFrames;

/* declaracao antecipada para o snapshot poder usar cb_coleta_lista */
static void cb_coleta_lista(void *elemento, void *aux);

/*
 * snapshot_frame — callback chamado a cada passo do algoritmo.
 * Gera um SVG numerado mostrando o estado atual do vetor.
 * Os dois elementos comparados/trocados (i e j) sao destacados
 * com circulo azul na ancora.
 */
static void snapshot_frame(void **vetor, int n, int i, int j, void *ctx)
{
    CtxFrames *c = (CtxFrames *)ctx;

    char caminho[MAX_PATH * 5];
    snprintf(caminho, sizeof(caminho), "%s%06d.svg", c->base, c->contador++);

    Lista *todas = lista_criar();
    emOrdemArvore(c->formas, cb_coleta_lista, todas);

    FILE *arq = fopen(caminho, "w");
    if (!arq)
    {
        lista_destruir(todas);
        return;
    }

    double l, a;
    svg_calcula_dimensoes(todas, &l, &a);
    svg_abre(arq, l, a);
    svg_desenha_lista(arq, todas);

    /* destaca os dois elementos do passo atual com circulo azul */
    for (int idx = 0; idx < n; idx++)
    {
        if (idx == i || (j >= 0 && idx == j))
        {
            Forma *f = (Forma *)vetor[idx];
            double sx = forma_get_x(f) + 20.0;
            double sy = forma_get_y(f) + 20.0;
            fprintf(arq,
                    "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"5\""
                    " style=\"fill:none;stroke:blue;stroke-width:2\"/>\n",
                    sx, sy);
        }
    }

    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/* =============================================================
 * HELPERS INTERNOS — SVG COM ARVORE
 * O modulo SVG so conhece Lista*. Esses helpers fazem a ponte
 * entre a Arvore e o SVG sem acoplar os dois modulos.
 * ============================================================= */

/*
 * cb_coleta_lista — callback para emOrdemArvore.
 * Insere cada forma visitada na lista auxiliar.
 */
static void cb_coleta_lista(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

/*
 * qry_svg_de_arvore — gera o SVG com todas as formas em ordem.
 */
void qry_svg_de_arvore(const char *caminho, Arvore formas)
{
    Lista *temp = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, temp);
    svg_gera_arquivo((char *)caminho, temp);
    lista_destruir(temp);
}

/*
 * CtxSvgSel — contexto para gerar o SVG do sel.
 */
typedef struct
{
    Lista *selecionadas;
    double sel_x, sel_y, sel_w, sel_h;
} CtxSvgSel;

/*
 * svg_com_selecao — SVG com regiao pontilhada e anel nas ancoras selecionadas.
 */
static void svg_com_selecao(const char *caminho, Arvore formas, CtxSvgSel *ctx)
{
    Lista *todas = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, todas);

    FILE *arq = fopen(caminho, "w");
    if (!arq)
    {
        lista_destruir(todas);
        return;
    }

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
 * svg_com_remocao — SVG com X vermelho nas ancoras que serao removidas.
 */
static void svg_com_remocao(const char *caminho, Arvore formas,
                            Lista *selecionadas)
{
    Lista *todas = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, todas);

    FILE *arq = fopen(caminho, "w");
    if (!arq)
    {
        lista_destruir(todas);
        return;
    }

    double l, a;
    svg_calcula_dimensoes(todas, &l, &a);
    svg_abre(arq, l, a);
    svg_desenha_lista(arq, todas);

    int n = lista_tamanho(selecionadas);
    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(selecionadas, i);
        svg_desenha_x_remocao(arq, forma_get_x(f), forma_get_y(f), a);
    }

    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/* =============================================================
 * PREDICADO DE SELECAO
 * ============================================================= */

typedef struct
{
    double x, y, w, h;
} RegiaoSel;

/*
 * predicado_na_regiao — retorna 1 se a ancora esta dentro da regiao.
 * Para poligonos, basta um vertice estar dentro.
 */
static int predicado_na_regiao(void *elemento, void *ctx)
{
    Forma *f = (Forma *)elemento;
    RegiaoSel *r = (RegiaoSel *)ctx;

    if (forma_get_tipo(f) == FORMA_POLIGONO)
    {
        int n = forma_get_num_pontos(f);
        for (int k = 0; k < n; k++)
        {
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

static void cb_coleta_selecionadas(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

/* =============================================================
 * PROCESSAMENTO DO .QRY
 * ============================================================= */

void qry_processa_arquivo(FILE *arq_qry, Arvore formas,
                          Poligono **poligonos, FILE *arq_txt,
                          const char *base_svg)
{
    char cmd[8];
    Lista *selecionadas = lista_criar();

    while (fscanf(arq_qry, "%7s", cmd) == 1)
    {

        if (strcmp(cmd, "inp") == 0)
        {
            /* inp p i — insere ancora da figura i no poligono p */
            int p, i;
            fscanf(arq_qry, "%d %d", &p, &i);
            if (p < 1 || p > MAX_POLIGONOS)
                continue;

            Forma *f = (Forma *)buscarPorIdArvore(formas, i, geo_get_id);
            if (f == NULL)
                continue;

            double ax = forma_get_x(f), ay = forma_get_y(f);
            if (forma_get_tipo(f) == FORMA_LINHA)
            {
                double x2 = forma_get_x2(f), y2 = forma_get_y2(f);
                if (x2 < ax || (x2 == ax && y2 < ay))
                {
                    ax = x2;
                    ay = y2;
                }
            }
            pol_insere(poligonos[p - 1], ax, ay, i);
            if (arq_txt)
                fprintf(arq_txt, "[*] inp %d %d -> (%.2f, %.2f)\n", p, i, ax, ay);
        }
        else if (strcmp(cmd, "rmp") == 0)
        {
            /* rmp p — remove a ancora mais antiga do poligono p */
            int p;
            fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS)
                continue;
            double ox, oy;
            int oid;
            if (pol_remove(poligonos[p - 1], &ox, &oy, &oid))
                if (arq_txt)
                    fprintf(arq_txt, "[*] rmp %d: removido pt de id %d\n", p, oid);
        }
        else if (strcmp(cmd, "clp") == 0)
        {
            /* clp p — limpa todas as coordenadas do poligono p */
            int p;
            fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS)
                continue;
            pol_limpa(poligonos[p - 1]);
            if (arq_txt)
                fprintf(arq_txt, "[*] clp %d: poligono esvaziado\n", p);
        }
        else if (strcmp(cmd, "pol") == 0)
        {
            /* pol p id esp corb corp — gera o poligono p e insere na arvore */
            int p, id_pol;
            double esp;
            char corb[32], corp[32];
            fscanf(arq_qry, "%d %d %lf %31s %31s", &p, &id_pol, &esp, corb, corp);
            if (p >= 1 && p <= MAX_POLIGONOS)
            {
                int n_pts = pol_tamanho(poligonos[p - 1]);
                if (n_pts >= 3)
                {
                    double *px = (double *)malloc(n_pts * sizeof(double));
                    double *py = (double *)malloc(n_pts * sizeof(double));
                    int oid;
                    for (int k = 0; k < n_pts; k++)
                        pol_get(poligonos[p - 1], k, &px[k], &py[k], &oid);
                    Forma *f = forma_cria_poligono(id_pol, n_pts, px, py, corb, corp);
                    if (f)
                        inserirArvore(formas, f);
                    free(px);
                    free(py);
                }
            }
            if (arq_txt)
                fprintf(arq_txt, "[*] pol %d: gerado com id %d\n", p, id_pol);
        }
        else if (strcmp(cmd, "sel") == 0)
        {
            /* sel x y w h — seleciona formas cuja ancora esta dentro da regiao
             * TXT: reporta id e tipo de cada selecionada
             * SVG: regiao pontilhada vermelha + anel nas ancoras */
            RegiaoSel reg;
            fscanf(arq_qry, "%lf %lf %lf %lf", &reg.x, &reg.y, &reg.w, &reg.h);

            lista_destruir(selecionadas);
            selecionadas = lista_criar();

            emOrdemFiltroArvore(formas,
                                predicado_na_regiao, &reg,
                                cb_coleta_selecionadas, selecionadas);

            if (arq_txt)
            {
                fprintf(arq_txt, "[*] sel %.1f %.1f %.1f %.1f\n",
                        reg.x, reg.y, reg.w, reg.h);
                int n = lista_tamanho(selecionadas);
                for (int i = 0; i < n; i++)
                {
                    Forma *f = (Forma *)lista_get(selecionadas, i);
                    fprintf(arq_txt, "   - id %d tipo %d\n",
                            forma_get_id(f), (int)forma_get_tipo(f));
                }
            }

            if (base_svg != NULL)
            {
                CtxSvgSel ctx_svg = {
                    selecionadas,
                    reg.x, reg.y, reg.w, reg.h};
                svg_com_selecao(base_svg, formas, &ctx_svg);
            }
        }
        else if (strcmp(cmd, "dels") == 0)
        {
            /* dels — remove da arvore todas as formas selecionadas
             * SVG: marca com X vermelho antes de remover */
            int n = lista_tamanho(selecionadas);

            if (base_svg != NULL && n > 0)
                svg_com_remocao(base_svg, formas, selecionadas);

            for (int i = 0; i < n; i++)
            {
                Forma *f = (Forma *)lista_get(selecionadas, i);
                if (arq_txt)
                    fprintf(arq_txt, "[*] dels: removido id %d\n", forma_get_id(f));
                removerArvore(formas, f);
                forma_destroi(f);
            }
            lista_destruir(selecionadas);
            selecionadas = lista_criar();
        }
        else if (strcmp(cmd, "find") == 0 || strcmp(cmd, "findrm") == 0)
        {
            /* find  k alg crit x y dw — ordena, gera frames, posiciona k menores
             * findrm k alg crit x y dw — igual, mas remove os de rank > k
             *
             * TXT: id, tipo e atributo relevante de cada forma em ordem
             * SVG: quadrado vermelho na ancora dos k menores */
            int eh_findrm = (strcmp(cmd, "findrm") == 0);
            int k;
            char alg[8], crit[4];
            double dest_x, dest_y, dw;
            fscanf(arq_qry, "%d %7s %3s %lf %lf %lf",
                   &k, alg, crit, &dest_x, &dest_y, &dw);

            int n_sel = lista_tamanho(selecionadas);
            if (n_sel == 0)
                continue;

            /* monta vetor de ponteiros com as selecionadas */
            void **vetor = (void **)malloc(n_sel * sizeof(void *));
            for (int i = 0; i < n_sel; i++)
                vetor[i] = lista_get(selecionadas, i);

            /* escolhe o comparador pelo criterio */
            FuncaoComparacaoSort cmp_sort = NULL;
            if (strcmp(crit, "d") == 0)
                cmp_sort = (FuncaoComparacaoSort)geo_comparar;
            else if (strcmp(crit, "a") == 0)
                cmp_sort = (FuncaoComparacaoSort)geo_comparar_area;
            else if (strcmp(crit, "w") == 0)
                cmp_sort = (FuncaoComparacaoSort)geo_comparar_largura;
            else if (strcmp(crit, "h") == 0)
                cmp_sort = (FuncaoComparacaoSort)geo_comparar_altura;
            else if (strcmp(crit, "c") == 0)
                cmp_sort = (FuncaoComparacaoSort)geo_comparar_cor;
            if (!cmp_sort)
            {
                free(vetor);
                continue;
            }

            /* monta o caminho base dos frames sem extensao
             * ex: "saida/arq-qry.svg" vira "saida/arq-qry" */
            CtxFrames ctx_frames;
            if (base_svg)
            {
                strncpy(ctx_frames.base, base_svg, MAX_PATH * 4 - 1);
                ctx_frames.base[MAX_PATH * 4 - 1] = '\0';
                char *ponto = strrchr(ctx_frames.base, '.');
                if (ponto)
                    *ponto = '\0';
            }
            ctx_frames.contador = 1;
            ctx_frames.formas = formas;

            /* executa o algoritmo escolhido gerando um frame por troca */
            FuncaoSnapshot snap = base_svg ? snapshot_frame : NULL;
            if (strcmp(alg, "bs") == 0)
                sort_bubble(vetor, n_sel, cmp_sort, snap, &ctx_frames);
            else if (strcmp(alg, "ss") == 0)
                sort_selection(vetor, n_sel, cmp_sort, snap, &ctx_frames);
            else if (strcmp(alg, "is") == 0)
                sort_insertion(vetor, n_sel, cmp_sort, snap, &ctx_frames);
            else if (strcmp(alg, "shs") == 0)
                sort_shell(vetor, n_sel, cmp_sort, snap, &ctx_frames);
            else if (strcmp(alg, "qs") == 0)
                sort_quick(vetor, n_sel, cmp_sort, snap, &ctx_frames);
            else if (strcmp(alg, "ms") == 0)
                sort_merge(vetor, n_sel, cmp_sort, snap, &ctx_frames);

            /* limita k ao tamanho real */
            if (k > n_sel)
                k = n_sel;

            /* TXT: reporta id, tipo e atributo relevante em ordem */
            if (arq_txt)
            {
                fprintf(arq_txt, "[*] %s %d %s %s %.1f %.1f %.1f\n",
                        cmd, k, alg, crit, dest_x, dest_y, dw);
                for (int i = 0; i < n_sel; i++)
                {
                    Forma *f = (Forma *)vetor[i];
                    fprintf(arq_txt, "   %d: id=%d tipo=%d",
                            i + 1, forma_get_id(f), (int)forma_get_tipo(f));
                    if (strcmp(crit, "a") == 0)
                        fprintf(arq_txt, " area=%.2f", geo_area(f));
                    else if (strcmp(crit, "w") == 0)
                        fprintf(arq_txt, " largura=%.2f", geo_largura(f));
                    else if (strcmp(crit, "h") == 0)
                        fprintf(arq_txt, " altura=%.2f", geo_altura(f));
                    else if (strcmp(crit, "c") == 0)
                        fprintf(arq_txt, " cor=%s",
                                forma_get_tipo(f) == FORMA_LINHA
                                    ? forma_get_cor_borda(f)
                                    : forma_get_cor_preench(f));
                    fprintf(arq_txt, "\n");
                }
            }

            /* reposiciona os k menores a partir de (dest_x, dest_y) com passo dw
             * remove da arvore, atualiza posicao, reinsere — mantem ABB valida */
            double pos_x = dest_x;
            for (int i = 0; i < k; i++)
            {
                Forma *f = (Forma *)vetor[i];
                removerArvore(formas, f);
                /* para linha, preserva o deslocamento relativo entre os dois pontos */
                if (forma_get_tipo(f) == FORMA_LINHA)
                {
                    double ddx = forma_get_x2(f) - forma_get_x(f);
                    double ddy = forma_get_y2(f) - forma_get_y(f);
                    forma_set_x2(f, pos_x + ddx);
                    forma_set_y2(f, dest_y + ddy);
                }
                forma_set_x(f, pos_x);
                forma_set_y(f, dest_y);
                inserirArvore(formas, f);
                pos_x += dw;
            }

            /* findrm: remove da arvore os elementos de rank > k */
            if (eh_findrm)
            {
                for (int i = k; i < n_sel; i++)
                {
                    Forma *f = (Forma *)vetor[i];
                    removerArvore(formas, f);
                    forma_destroi(f);
                }
            }

            /* SVG final: quadrado vermelho na ancora dos k menores */
            if (base_svg)
            {
                Lista *todas = lista_criar();
                emOrdemArvore(formas, cb_coleta_lista, todas);
                FILE *arq_svg = fopen(base_svg, "w");
                if (arq_svg)
                {
                    double l, a;
                    svg_calcula_dimensoes(todas, &l, &a);
                    svg_abre(arq_svg, l, a);
                    svg_desenha_lista(arq_svg, todas);
                    for (int i = 0; i < k; i++)
                    {
                        Forma *f = (Forma *)vetor[i];
                        svg_desenha_quadrado_selecao(arq_svg,
                                                     forma_get_x(f),
                                                     forma_get_y(f), a);
                    }
                    svg_fecha(arq_svg);
                    fclose(arq_svg);
                }
                lista_destruir(todas);
            }

            free(vetor);
            lista_destruir(selecionadas);
            selecionadas = lista_criar();
        }
        else if (strcmp(cmd, "mcs") == 0)
        {
            /* mcs dx dy corb corp — translada e recolore as selecionadas
             * ao mover, a chave na ABB muda: remover -> atualizar -> reinserir */
            double dx, dy;
            char corb[32], corp[32];
            fscanf(arq_qry, "%lf %lf %31s %31s", &dx, &dy, corb, corp);

            int n = lista_tamanho(selecionadas);
            for (int i = 0; i < n; i++)
            {
                Forma *f = (Forma *)lista_get(selecionadas, i);

                removerArvore(formas, f);

                forma_set_x(f, forma_get_x(f) + dx);
                forma_set_y(f, forma_get_y(f) + dy);

                if (forma_get_tipo(f) == FORMA_LINHA)
                {
                    forma_set_x2(f, forma_get_x2(f) + dx);
                    forma_set_y2(f, forma_get_y2(f) + dy);
                }
                else if (forma_get_tipo(f) == FORMA_POLIGONO)
                {
                    int n_pts = forma_get_num_pontos(f);
                    for (int j = 0; j < n_pts; j++)
                    {
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
        else if (strcmp(cmd, "mc") == 0)
        {
            /* mc corb corp — muda cor da borda e do preenchimento das selecionadas
             * cor nao faz parte da chave (y, x, area), entao nao precisa
             * remover e reinserir na arvore */
            char corb[32], corp[32];
            fscanf(arq_qry, "%31s %31s", corb, corp);

            int n = lista_tamanho(selecionadas);
            for (int i = 0; i < n; i++)
            {
                Forma *f = (Forma *)lista_get(selecionadas, i);
                forma_set_cor_borda(f, corb);
                forma_set_cor_preench(f, corp);
            }
            if (arq_txt)
                fprintf(arq_txt, "[*] mc: cores alteradas em %d formas\n", n);
        }
        else if (strcmp(cmd, "cm") == 0)
        {
            /* cm x y w h dx dy — clona formas da regiao, move os clones
             * e os torna a nova selecao.
             * (Desconsidera selecoes anteriores) */
            RegiaoSel reg;
            double dx, dy;
            fscanf(arq_qry, "%lf %lf %lf %lf %lf %lf",
                   &reg.x, &reg.y, &reg.w, &reg.h, &dx, &dy);

            /* coleta as formas dentro da regiao */
            Lista *originais = lista_criar();
            emOrdemFiltroArvore(formas,
                                predicado_na_regiao, &reg,
                                cb_coleta_selecionadas, originais);

            /* descarta selecao anterior */
            lista_destruir(selecionadas);
            selecionadas = lista_criar();

            int n = lista_tamanho(originais);
            for (int i = 0; i < n; i++)
            {
                Forma *orig = (Forma *)lista_get(originais, i);
                Forma *clone = forma_clona(orig);
                if (clone == NULL)
                    continue;

                /* move o clone pelo deslocamento */
                forma_set_x(clone, forma_get_x(clone) + dx);
                forma_set_y(clone, forma_get_y(clone) + dy);

                if (forma_get_tipo(clone) == FORMA_LINHA)
                {
                    forma_set_x2(clone, forma_get_x2(clone) + dx);
                    forma_set_y2(clone, forma_get_y2(clone) + dy);
                }
                else if (forma_get_tipo(clone) == FORMA_POLIGONO)
                {
                    int n_pts = forma_get_num_pontos(clone);
                    for (int j = 0; j < n_pts; j++)
                    {
                        forma_set_ponto_x(clone, j, forma_get_ponto_x(clone, j) + dx);
                        forma_set_ponto_y(clone, j, forma_get_ponto_y(clone, j) + dy);
                    }
                }

                inserirArvore(formas, clone);
                lista_inserir_fim(selecionadas, clone);
            }

            if (arq_txt)
                fprintf(arq_txt, "[*] cm: %d formas clonadas\n", n);

            lista_destruir(originais);
        }
    }
        lista_destruir(selecionadas);
}