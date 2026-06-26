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
 * TIPOS INTERNOS
 * ============================================================= */

/*
 * RegiaoSel — retangulo de selecao usado pelo predicado e pelo cm.
 */
typedef struct
{
    double x, y, w, h;
} RegiaoSel;

/*
 * CtxFrames — contexto passado ao callback de snapshot do sort.
 * base    : caminho base sem extensao (ex: "saida/arq-qry")
 * contador: proximo numero de frame, comeca em 1
 * formas  : arvore com todas as formas (fundo de cada frame)
 * dest_x, dest_y, dw : posicao e espacamento de destino do find,
 * usados para posicionar o vetor sendo ordenado
 * em cada frame da animacao
 */
typedef struct
{
    char base[MAX_PATH * 4];
    int contador;
    Arvore formas;
    double dest_x, dest_y, dw;
} CtxFrames;

/* =============================================================
 * CALLBACKS E PREDICADOS
 * ============================================================= */

/* insere cada forma visitada na lista auxiliar */
static void cb_coleta_lista(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

/* insere na lista de selecionadas */
static void cb_coleta_sel(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

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

/*
 * snapshot_frame — gera um SVG numerado a cada passo do algoritmo.
 *
 * Diferente de desenhar a arvore com as posicoes originais (que nao
 * mudam durante o sort), este snapshot desenha as formas do VETOR
 * já posicionadas na fileira de destino (dest_x, dest_y, dw), na
 * ordem atual do vetor naquele instante. Assim cada frame mostra a
 * troca de posicao entre elementos, como uma animacao real de
 * reorganizacao — analogo ao exemplo do professor (bolinha se
 * deslocando sobre um cenario fixo).
 *
 * As formas que nao fazem parte do vetor (nao foram selecionadas)
 * continuam aparecendo no fundo, nas posicoes originais da arvore.
 *
 * Os dois elementos comparados/trocados no passo atual (i e j)
 * ganham um anel azul de destaque.
 */
static void snapshot_frame(void **vetor, int n, int i, int j, void *ctx)
{
    CtxFrames *c = (CtxFrames *)ctx;

    char caminho[MAX_PATH * 5];
    snprintf(caminho, sizeof(caminho), "%s%06d.svg", c->base, c->contador++);

    /* fundo: todas as formas da arvore (inclui as do vetor, nas
     * posicoes antigas — serao sobrepostas pelo desenho do vetor) */
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

    /* desenha o vetor na fileira de destino, na ordem atual */
    double pos_x = c->dest_x;
    for (int idx = 0; idx < n; idx++)
    {
        Forma *f = (Forma *)vetor[idx];

        /* clona temporariamente so para desenhar na posicao da fileira
         * sem alterar a forma real (que so deve mudar ao final do find) */
        Forma *temp = forma_clona(f);
        if (temp)
        {
            forma_set_x(temp, pos_x);
            forma_set_y(temp, c->dest_y);
            svg_desenha_forma(arq, temp, a);

            /* anel azul nos dois elementos do passo atual */
            if (idx == i || (j >= 0 && idx == j))
            {
                fprintf(arq,
                        "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"8\""
                        " style=\"fill:none;stroke:blue;stroke-width:2\"/>\n",
                        pos_x + 20.0, c->dest_y + 20.0);
            }
            forma_destroi(temp);
        }
        pos_x += c->dw;
    }

    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/* =============================================================
 * HELPERS SVG
 * ============================================================= */

/*
 * qry_svg_de_arvore — gera SVG com todas as formas da arvore em ordem.
 */
void qry_svg_de_arvore(const char *caminho, Arvore formas)
{
    Lista *temp = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, temp);
    svg_gera_arquivo((char *)caminho, temp);
    lista_destruir(temp);
}

/*
 * svg_de_arvore_com_sel — SVG com regiao pontilhada e anel nas ancoras.
 */
static void svg_de_arvore_com_sel(const char *caminho, Arvore formas,
                                  double sx, double sy, double sw, double sh,
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
    svg_desenha_selecao(arq, sx, sy, sw, sh, selecionadas, a);
    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/*
 * svg_de_arvore_com_x — SVG com X vermelho nas ancoras a remover.
 */
static void svg_de_arvore_com_x(const char *caminho, Arvore formas,
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

/*
 * svg_de_arvore_com_quadrados — SVG final do find com quadrado vermelho
 * na ancora dos k menores.
 */
static void svg_de_arvore_com_quadrados(const char *caminho, Arvore formas,
                                        void **vetor, int k)
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
    for (int i = 0; i < k; i++)
    {
        Forma *f = (Forma *)vetor[i];
        svg_desenha_quadrado_selecao(arq, forma_get_x(f), forma_get_y(f), a);
    }
    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/* =============================================================
 * HELPER: move uma forma atualizando todos os seus pontos
 * ============================================================= */

static void move_forma(Forma *f, double dx, double dy)
{
    forma_set_x(f, forma_get_x(f) + dx);
    forma_set_y(f, forma_get_y(f) + dy);
    if (forma_get_tipo(f) == FORMA_LINHA)
    {
        forma_set_x2(f, forma_get_x2(f) + dx);
        forma_set_y2(f, forma_get_y2(f) + dy);
    }
    else if (forma_get_tipo(f) == FORMA_POLIGONO)
    {
        int n = forma_get_num_pontos(f);
        for (int i = 0; i < n; i++)
        {
            forma_set_ponto_x(f, i, forma_get_ponto_x(f, i) + dx);
            forma_set_ponto_y(f, i, forma_get_ponto_y(f, i) + dy);
        }
    }
}

/* =============================================================
 * IMPLEMENTACAO DOS COMANDOS
 * cada funcao le os seus parametros do arquivo e executa a logica
 * ============================================================= */

static void cmd_inp(FILE *arq_qry, Arvore formas,
                    Poligono **poligonos, FILE *arq_txt)
{
    int p, i;
    fscanf(arq_qry, "%d %d", &p, &i);
    if (p < 1 || p > MAX_POLIGONOS)
        return;

    Forma *f = (Forma *)buscarPorIdArvore(formas, i, geo_get_id);
    if (!f)
        return;

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

static void cmd_rmp(FILE *arq_qry, Poligono **poligonos, FILE *arq_txt)
{
    int p;
    fscanf(arq_qry, "%d", &p);
    if (p < 1 || p > MAX_POLIGONOS)
        return;
    double ox, oy;
    int oid;
    if (pol_remove(poligonos[p - 1], &ox, &oy, &oid))
        if (arq_txt)
            fprintf(arq_txt, "[*] rmp %d: removido pt de id %d\n", p, oid);
}

static void cmd_clp(FILE *arq_qry, Poligono **poligonos, FILE *arq_txt)
{
    int p;
    fscanf(arq_qry, "%d", &p);
    if (p < 1 || p > MAX_POLIGONOS)
        return;
    pol_limpa(poligonos[p - 1]);
    if (arq_txt)
        fprintf(arq_txt, "[*] clp %d: poligono esvaziado\n", p);
}

static void cmd_pol(FILE *arq_qry, Arvore formas,
                    Poligono **poligonos, FILE *arq_txt)
{
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

static void cmd_sel(FILE *arq_qry, Arvore formas,
                    Lista **selecionadas,
                    FILE *arq_txt, const char *base_svg)
{
    RegiaoSel reg;
    fscanf(arq_qry, "%lf %lf %lf %lf", &reg.x, &reg.y, &reg.w, &reg.h);

    lista_destruir(*selecionadas);
    *selecionadas = lista_criar();

    emOrdemFiltroArvore(formas, predicado_na_regiao, &reg,
                        cb_coleta_sel, *selecionadas);

    if (arq_txt)
    {
        fprintf(arq_txt, "[*] sel %.1f %.1f %.1f %.1f\n",
                reg.x, reg.y, reg.w, reg.h);
        int n = lista_tamanho(*selecionadas);
        for (int i = 0; i < n; i++)
        {
            Forma *f = (Forma *)lista_get(*selecionadas, i);
            fprintf(arq_txt, "   - id %d tipo %d\n",
                    forma_get_id(f), (int)forma_get_tipo(f));
        }
    }

    if (base_svg)
        svg_de_arvore_com_sel(base_svg, formas,
                              reg.x, reg.y, reg.w, reg.h, *selecionadas);
}

static void cmd_dels(Arvore formas, Lista **selecionadas,
                     FILE *arq_txt, const char *base_svg)
{
    int n = lista_tamanho(*selecionadas);
    if (base_svg && n > 0)
        svg_de_arvore_com_x(base_svg, formas, *selecionadas);

    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(*selecionadas, i);
        if (arq_txt)
            fprintf(arq_txt, "[*] dels: removido id %d\n", forma_get_id(f));
        removerArvore(formas, f);
        forma_destroi(f);
    }
    lista_destruir(*selecionadas);
    *selecionadas = lista_criar();
}

static void cmd_mcs(FILE *arq_qry, Arvore formas,
                    Lista *selecionadas, FILE *arq_txt, const char *base_svg)
{
    double dx, dy;
    char corb[32], corp[32];
    fscanf(arq_qry, "%lf %lf %31s %31s", &dx, &dy, corb, corp);

    int n = lista_tamanho(selecionadas);
    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(selecionadas, i);
        removerArvore(formas, f);
        move_forma(f, dx, dy);
        forma_set_cor_borda(f, corb);
        forma_set_cor_preench(f, corp);
        inserirArvore(formas, f);
    }
    if (arq_txt)
        fprintf(arq_txt, "[*] mcs: transladadas %d formas\n", n);

    /* --- LÓGICA DE GERAR FRAMES --- */
    if (base_svg)
    {
        static int contador_mcs = 1;
        char prefixo[MAX_PATH];
        char caminho[MAX_PATH * 2];
        
        strncpy(prefixo, base_svg, sizeof(prefixo) - 1);
        prefixo[sizeof(prefixo) - 1] = '\0';
        
        char *ponto = strrchr(prefixo, '.');
        if (ponto) *ponto = '\0';
        
        snprintf(caminho, sizeof(caminho), "%s_frame_%03d.svg", prefixo, contador_mcs++);
        
        qry_svg_de_arvore(caminho, formas);
    }
}

static void cmd_mc(FILE *arq_qry, Lista *selecionadas, FILE *arq_txt)
{
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

static void cmd_cm(FILE *arq_qry, Arvore formas,
                   Lista **selecionadas, FILE *arq_txt)
{
    RegiaoSel reg;
    double dx, dy;
    fscanf(arq_qry, "%lf %lf %lf %lf %lf %lf",
           &reg.x, &reg.y, &reg.w, &reg.h, &dx, &dy);

    Lista *originais = lista_criar();
    emOrdemFiltroArvore(formas, predicado_na_regiao, &reg,
                        cb_coleta_sel, originais);

    lista_destruir(*selecionadas);
    *selecionadas = lista_criar();

    int n = lista_tamanho(originais);
    for (int i = 0; i < n; i++)
    {
        Forma *orig = (Forma *)lista_get(originais, i);
        Forma *clone = forma_clona(orig);
        if (!clone)
            continue;
        move_forma(clone, dx, dy);
        inserirArvore(formas, clone);
        lista_inserir_fim(*selecionadas, clone);
    }
    if (arq_txt)
        fprintf(arq_txt, "[*] cm: %d formas clonadas\n", n);
    lista_destruir(originais);
}

/*
 * cmd_find — implementa find e findrm.
 * eh_findrm: 0 para find, 1 para findrm
 */
static void cmd_find(FILE *arq_qry, Arvore formas,
                     Lista **selecionadas,
                     FILE *arq_txt, const char *base_svg,
                     int eh_findrm)
{
    int k;
    char alg[8], crit[4];
    double dest_x, dest_y, dw;
    fscanf(arq_qry, "%d %7s %3s %lf %lf %lf",
           &k, alg, crit, &dest_x, &dest_y, &dw);

    int n_sel = lista_tamanho(*selecionadas);
    if (n_sel == 0)
        return;

    /* monta vetor de ponteiros */
    void **vetor = (void **)malloc(n_sel * sizeof(void *));
    for (int i = 0; i < n_sel; i++)
        vetor[i] = lista_get(*selecionadas, i);

    /* escolhe o comparador */
    FuncaoComparacaoSort cmp = NULL;
    if (strcmp(crit, "d") == 0)
        cmp = (FuncaoComparacaoSort)geo_comparar;
    else if (strcmp(crit, "a") == 0)
        cmp = (FuncaoComparacaoSort)geo_comparar_area;
    else if (strcmp(crit, "w") == 0)
        cmp = (FuncaoComparacaoSort)geo_comparar_largura;
    else if (strcmp(crit, "h") == 0)
        cmp = (FuncaoComparacaoSort)geo_comparar_altura;
    else if (strcmp(crit, "c") == 0)
        cmp = (FuncaoComparacaoSort)geo_comparar_cor;
    if (!cmp)
    {
        free(vetor);
        return;
    }

    /* prepara o contexto dos frames — inclui a posicao de destino
     * para que cada frame desenhe o vetor ja na fileira final */
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
    ctx_frames.dest_x = dest_x;
    ctx_frames.dest_y = dest_y;
    ctx_frames.dw = dw;

    /* gera o frame 0 (estado inicial, antes de qualquer troca) para
     * a animacao comecar mostrando a fileira desordenada */
    FuncaoSnapshot snap = base_svg ? snapshot_frame : NULL;
    if (snap)
        snap(vetor, n_sel, -1, -1, &ctx_frames);

    /* executa o algoritmo */
    if (strcmp(alg, "bs") == 0)
        sort_bubble(vetor, n_sel, cmp, snap, &ctx_frames);
    else if (strcmp(alg, "ss") == 0)
        sort_selection(vetor, n_sel, cmp, snap, &ctx_frames);
    else if (strcmp(alg, "is") == 0)
        sort_insertion(vetor, n_sel, cmp, snap, &ctx_frames);
    else if (strcmp(alg, "shs") == 0)
        sort_shell(vetor, n_sel, cmp, snap, &ctx_frames);
    else if (strcmp(alg, "qs") == 0)
        sort_quick(vetor, n_sel, cmp, snap, &ctx_frames);
    else if (strcmp(alg, "ms") == 0)
        sort_merge(vetor, n_sel, cmp, snap, &ctx_frames);

    if (k > n_sel)
        k = n_sel;

    /* TXT: id, tipo e atributo relevante em ordem */
    if (arq_txt)
    {
        fprintf(arq_txt, "[*] %s %d %s %s %.1f %.1f %.1f\n",
                eh_findrm ? "findrm" : "find",
                k, alg, crit, dest_x, dest_y, dw);
        for (int i = 0; i < k; i++)
        {
            Forma *f = (Forma *)vetor[i];
            fprintf(arq_txt, "   %d: id=%d tipo=%d", i + 1,
                    forma_get_id(f), (int)forma_get_tipo(f));
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

    /* reposiciona os k menores: remove, move, reinsere */
    double pos_x = dest_x;
    for (int i = 0; i < k; i++)
    {
        Forma *f = (Forma *)vetor[i];
        removerArvore(formas, f);
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

    /* findrm: remove os de rank > k */
    if (eh_findrm)
    {
        for (int i = k; i < n_sel; i++)
        {
            Forma *f = (Forma *)vetor[i];
            removerArvore(formas, f);
            forma_destroi(f);
        }
    }

    /* SVG final com quadrado vermelho nos k menores */
    if (base_svg)
        svg_de_arvore_com_quadrados(base_svg, formas, vetor, k);

    lista_destruir(*selecionadas);
    *selecionadas = lista_criar();

    if (!eh_findrm)
    {
        for (int i = 0; i < k; i++)
            lista_inserir_fim(*selecionadas, vetor[i]);
    }
    free(vetor);
}

/* =============================================================
 * DISPATCHER PRINCIPAL
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
            cmd_inp(arq_qry, formas, poligonos, arq_txt);
        else if (strcmp(cmd, "rmp") == 0)
            cmd_rmp(arq_qry, poligonos, arq_txt);
        else if (strcmp(cmd, "clp") == 0)
            cmd_clp(arq_qry, poligonos, arq_txt);
        else if (strcmp(cmd, "pol") == 0)
            cmd_pol(arq_qry, formas, poligonos, arq_txt);
        else if (strcmp(cmd, "sel") == 0)
            cmd_sel(arq_qry, formas, &selecionadas, arq_txt, base_svg);
        else if (strcmp(cmd, "dels") == 0)
            cmd_dels(formas, &selecionadas, arq_txt, base_svg);
        else if (strcmp(cmd, "mcs") == 0)
            cmd_mcs(arq_qry, formas, selecionadas, arq_txt, base_svg);
        else if (strcmp(cmd, "mc") == 0)
            cmd_mc(arq_qry, selecionadas, arq_txt);
        else if (strcmp(cmd, "cm") == 0)
            cmd_cm(arq_qry, formas, &selecionadas, arq_txt);
        else if (strcmp(cmd, "find") == 0)
            cmd_find(arq_qry, formas, &selecionadas, arq_txt, base_svg, 0);
        else if (strcmp(cmd, "findrm") == 0)
            cmd_find(arq_qry, formas, &selecionadas, arq_txt, base_svg, 1);
    }

    lista_destruir(selecionadas);
}