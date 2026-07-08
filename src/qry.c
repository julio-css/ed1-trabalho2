#define _POSIX_C_SOURCE 200809L

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
 * TIPOS AUXILIARES INTERNOS
 * ============================================================= */

/*
 * RegiaoSel – Define um retângulo de seleção utilizado pelos comandos
 * 'sel' e 'cm' para filtrar elementos com base em coordenadas espaciais.
 */
typedef struct
{
    double x, y, w, h;
} RegiaoSel;

/*
 * CtxFrames – Contexto passado à função de callback de snapshot durante
 * a ordenação. Armazena informações necessárias para gerar quadros
 * da animação, incluindo o diretório base, contador, árvore de formas,
 * posição de destino, largura do degrau, região de seleção e dimensões
 * da tela.
 * 
 * ALTERADO: adicionados os campos fundo_svg e fundo_tam para cache do fundo.
 */
typedef struct
{
    char base[MAX_PATH * 4];
    int contador;
    Arvore formas;
    double dest_x, dest_y, dw;
    RegiaoSel regiao;          /* retângulo de seleção para desenho */
    double largura_tela;
    double altura_tela;
    char *fundo_svg;           /* Buffer de memória com o fundo estático do SVG */
    size_t fundo_tam;          /* Tamanho do buffer do fundo */
} CtxFrames;

/* Variável estática para armazenar a última região de seleção,
 * utilizada para permitir o desenho do retângulo durante a animação
 * sem modificar a assinatura das funções de callback. */
static RegiaoSel ultima_regiao = {0, 0, 0, 0};

/* =============================================================
 * FUNÇÕES DE CALLBACK E PREDICADOS
 * ============================================================= */

/* Insere um elemento visitado em uma lista auxiliar. */
static void cb_coleta_lista(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

/* Insere um elemento na lista de formas selecionadas. */
static void cb_coleta_sel(void *elemento, void *aux)
{
    lista_inserir_fim((Lista *)aux, elemento);
}

/*
 * predicado_na_regiao – Verifica se a forma está completamente contida
 * no retângulo especificado. Implementação completa para todos os tipos.
 * ALTERADO: substituída pela versão do original que testa contêiner total.
 */
static int predicado_na_regiao(void *elemento, void *ctx)
{
    Forma *f = (Forma *)elemento;
    RegiaoSel *r = (RegiaoSel *)ctx;
    double x_min = r->x, x_max = r->x + r->w;
    double y_min = r->y, y_max = r->y + r->h;

    switch (forma_get_tipo(f))
    {
    case FORMA_CIRCULO:
    {
        double cx = forma_get_x(f), cy = forma_get_y(f), raio = forma_get_raio(f);
        return (cx - raio >= x_min && cx + raio <= x_max &&
                cy - raio >= y_min && cy + raio <= y_max);
    }
    case FORMA_RETANGULO:
    {
        double fx = forma_get_x(f), fy = forma_get_y(f);
        double w = forma_get_largura(f), h = forma_get_altura(f);
        return (fx >= x_min && fx + w <= x_max &&
                fy >= y_min && fy + h <= y_max);
    }
    case FORMA_LINHA:
    {
        double x1 = forma_get_x(f), y1 = forma_get_y(f);
        double x2 = forma_get_x2(f), y2 = forma_get_y2(f);
        return (x1 >= x_min && x1 <= x_max && y1 >= y_min && y1 <= y_max &&
                x2 >= x_min && x2 <= x_max && y2 >= y_min && y2 <= y_max);
    }
    case FORMA_TEXTO:
    {
        double fx = forma_get_x(f), fy = forma_get_y(f);
        char ancora = forma_get_ancora(f);
        int len = strlen(forma_get_texto(f));
        double largura = len * 1.0, altura = 10.0;
        double x0 = fx;
        if (ancora == 'm')
            x0 = fx - largura / 2.0;
        else if (ancora == 'f')
            x0 = fx - largura;
        return (x0 >= x_min && x0 + largura <= x_max &&
                fy - altura >= y_min && fy <= y_max);
    }
    case FORMA_POLIGONO:
    {
        int n = forma_get_num_pontos(f);
        for (int k = 0; k < n; k++)
        {
            double px = forma_get_ponto_x(f, k);
            double py = forma_get_ponto_y(f, k);
            if (px < x_min || px > x_max || py < y_min || py > y_max)
                return 0;
        }
        return 1;
    }
    default:
        return 0;
    }
}

/*
 * snapshot_frame – Gera um arquivo SVG numerado para cada passo do
 * algoritmo de ordenação.
 *
 * ALTERADO: utiliza o fundo estático cacheado (c->fundo_svg) quando disponível,
 * melhorando a performance. Mantém a lógica de desenho das formas na fileira.
 */
static void snapshot_frame(void **vetor, int n, int i, int j, void *ctx)
{
    CtxFrames *c = (CtxFrames *)ctx;
    char caminho[MAX_PATH * 5];
    snprintf(caminho, sizeof(caminho), "%s%06d.svg", c->base, c->contador++);

    FILE *arq = fopen(caminho, "w");
    if (!arq)
        return;

    double l = c->largura_tela;
    double a = c->altura_tela;

    svg_abre(arq, l, a);

    /* Desenha o retângulo vermelho pontilhado da seleção */
    Lista *vazia = lista_criar();
    svg_desenha_selecao(arq, c->regiao.x, c->regiao.y, c->regiao.w, c->regiao.h, vazia, a);
    lista_destruir(vazia);

    /* --- 1. Desenha o fundo (formas NÃO selecionadas) --- */
    if (c->fundo_svg)
    {
        /* Injeta o fundo pré‑renderizado (O(1) por frame) */
        fputs(c->fundo_svg, arq);
    }
    else
    {
        /* Fallback: percorre todas as formas e desenha as não selecionadas */
        Lista *todas = lista_criar();
        emOrdemArvore(c->formas, cb_coleta_lista, todas);
        int tam_todas = lista_tamanho(todas);
        for (int idx = 0; idx < tam_todas; idx++)
        {
            Forma *f = (Forma *)lista_get(todas, idx);
            int esta_no_vetor = 0;
            for (int k = 0; k < n; k++)
            {
                if (vetor[k] == f)
                {
                    esta_no_vetor = 1;
                    break;
                }
            }
            if (!esta_no_vetor)
            {
                svg_desenha_forma(arq, f, a);
            }
        }
        lista_destruir(todas);
    }

    /* --- 2. Desenha as formas selecionadas na fileira de destino --- */
    double pos_x = c->dest_x;
    for (int idx = 0; idx < n; idx++)
    {
        Forma *f = (Forma *)vetor[idx];

        double ox = forma_get_x(f);
        double oy = forma_get_y(f);
        double ox2 = 0, oy2 = 0;
        int e_linha = (forma_get_tipo(f) == FORMA_LINHA);

        if (e_linha)
        {
            ox2 = forma_get_x2(f);
            oy2 = forma_get_y2(f);
        }

        forma_set_x(f, pos_x);
        forma_set_y(f, c->dest_y);
        if (e_linha)
        {
            forma_set_x2(f, pos_x + 10.0);
            forma_set_y2(f, c->dest_y);
        }

        svg_desenha_forma(arq, f, a);

        /* Destaque azul para os elementos comparados/trocados */
        if (idx == i || (j >= 0 && idx == j))
        {
            fprintf(arq,
                    "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"8\""
                    " style=\"fill:none;stroke:blue;stroke-width:2\"/>\n",
                    pos_x + 20.0, c->dest_y + 20.0);
        }

        forma_set_x(f, ox);
        forma_set_y(f, oy);
        if (e_linha)
        {
            forma_set_x2(f, ox2);
            forma_set_y2(f, oy2);
        }

        pos_x += c->dw;
    }

    svg_fecha(arq);
    fclose(arq);
}

/* =============================================================
 * FUNÇÕES AUXILIARES PARA GERAÇÃO DE SVG
 * ============================================================= */

/*
 * qry_svg_de_arvore – Gera um arquivo SVG contendo todas as formas
 * da árvore, percorrendo-as em ordem simétrica.
 */
void qry_svg_de_arvore(const char *caminho, Arvore formas)
{
    Lista *temp = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, temp);
    svg_gera_arquivo((char *)caminho, temp);
    lista_destruir(temp);
}

/*
 * svg_de_arvore_com_sel – Gera SVG da árvore com a região de seleção
 * pontilhada e um círculo ao redor das âncoras das formas selecionadas.
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
 * svg_de_arvore_com_x – Gera SVG da árvore com um "X" vermelho sobre
 * as âncoras das formas que serão removidas (comando dels).
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
 * svg_de_arvore_com_quadrados – Gera SVG final do comando find,
 * destacando com um quadrado vermelho a âncora dos k menores elementos.
 * ALTERADO: agora recebe dest_x, dest_y, dw para desenhar o retângulo
 * tracejado laranja da fileira e utiliza viewBox para incluir toda a área.
 */
static void svg_de_arvore_com_quadrados(const char *caminho, Arvore formas,
                                        void **vetor, int k,
                                        double dest_x, double dest_y, double dw)
{
    Lista *todas = lista_criar();
    emOrdemArvore(formas, cb_coleta_lista, todas);
    FILE *arq = fopen(caminho, "w");
    if (!arq)
    {
        lista_destruir(todas);
        return;
    }

    /* Calcula a bounding box de todas as formas (incluindo a fileira) */
    double max_x = 0.0, max_y = 0.0;
    double min_x = 1e9, min_y = 1e9;
    int n = lista_tamanho(todas);
    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(todas, i);
        double x = forma_get_x(f), y = forma_get_y(f);
        double x2 = x, y2 = y;
        switch (forma_get_tipo(f))
        {
        case FORMA_CIRCULO:
            x2 = x + forma_get_raio(f);
            y2 = y + forma_get_raio(f);
            break;
        case FORMA_RETANGULO:
            x2 = x + forma_get_largura(f);
            y2 = y + forma_get_altura(f);
            break;
        case FORMA_LINHA:
        {
            double x2l = forma_get_x2(f), y2l = forma_get_y2(f);
            x2 = (x2l > x) ? x2l : x;
            y2 = (y2l > y) ? y2l : y;
            if (x < min_x) min_x = x;
            if (y < min_y) min_y = y;
            if (x2l > max_x) max_x = x2l;
            if (y2l > max_y) max_y = y2l;
            continue;
        }
        case FORMA_POLIGONO:
        {
            int num_pts = forma_get_num_pontos(f);
            for (int k2 = 0; k2 < num_pts; k2++)
            {
                double px = forma_get_ponto_x(f, k2);
                double py = forma_get_ponto_y(f, k2);
                if (px < min_x) min_x = px;
                if (py < min_y) min_y = py;
                if (px > max_x) max_x = px;
                if (py > max_y) max_y = py;
            }
            continue;
        }
        default:
            break;
        }
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x2 > max_x) max_x = x2;
        if (y2 > max_y) max_y = y2;
    }

    /* Inclui a área da fileira de destino */
    if (k > 0)
    {
        double largura_fileira = k * dw;
        double altura_fileira = 60.0; /* altura arbitrária para o retângulo */
        double fx = dest_x, fy = dest_y - 30.0;
        if (fx < min_x) min_x = fx;
        if (fy < min_y) min_y = fy;
        if (fx + largura_fileira > max_x) max_x = fx + largura_fileira;
        if (fy + altura_fileira > max_y) max_y = fy + altura_fileira;
    }

    double margem = 1.0;
    double view_x = min_x - margem, view_y = min_y - margem;
    double view_w = max_x - min_x + 2 * margem;
    double view_h = max_y - min_y + 2 * margem;

    svg_abre_com_viewbox(arq, view_w, view_h, view_x, view_y);
    svg_desenha_lista(arq, todas);

    /* Desenha o retângulo tracejado laranja da fileira */
    if (k > 0)
    {
        double largura_fileira = k * dw;
        fprintf(arq, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
                     "style=\"fill:none;stroke:orange;stroke-width:2;stroke-dasharray:8,4\"/>\n",
                dest_x, dest_y - 30.0, largura_fileira, 60.0);
    }

    /* Desenha os quadrados vermelhos sobre os k primeiros */
    for (int i = 0; i < k; i++)
    {
        Forma *f = (Forma *)vetor[i];
        svg_desenha_quadrado_selecao(arq, forma_get_x(f), forma_get_y(f), view_h);
    }

    svg_fecha(arq);
    fclose(arq);
    lista_destruir(todas);
}

/* =============================================================
 * FUNÇÃO AUXILIAR PARA MOVIMENTAÇÃO DE FORMAS
 * ============================================================= */

/* Aplica um deslocamento (dx, dy) a todos os pontos de uma forma,
 * incluindo linhas e polígonos. */
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
 * IMPLEMENTAÇÃO DOS COMANDOS DA QRV
 * ============================================================= */

/* Comando 'inp': insere um ponto (âncora) em um polígono identificado
 * pelo índice p. Utiliza a forma de id i como referência para a posição. */
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

/* Comando 'rmp': remove o último ponto inserido em um polígono. */
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

/* Comando 'clp': esvazia completamente um polígono. */
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

/* Comando 'pol': gera uma nova forma polígono a partir dos pontos
 * armazenados em um polígono auxiliar e a insere na árvore. */
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

/*
 * Comando 'sel': seleciona todas as formas cuja âncora (ou vértice,
 * no caso de polígonos) se encontra dentro da região retangular
 * especificada. A região é armazenada globalmente para uso em comandos
 * posteriores (especialmente para animação). Gera SVG intermediário
 * se solicitado.
 */
static void cmd_sel(FILE *arq_qry, Arvore formas,
                    Lista **selecionadas,
                    FILE *arq_txt, const char *base_svg)
{
    RegiaoSel reg;
    
    if (fscanf(arq_qry, "%lf %lf %lf %lf", &reg.x, &reg.y, &reg.w, &reg.h) != 4)
        return;

    ultima_regiao = reg;

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

/* Comando 'dels': remove da árvore todas as formas atualmente
 * selecionadas. Gera SVG com "X" sobre elas antes da remoção. */
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

/* Comando 'mcs': translada as formas selecionadas pelo vetor (dx, dy),
 * altera suas cores e gera um frame SVG da árvore resultante. */
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

    if (base_svg)
    {
        static int contador_mcs = 1;
        char prefixo[MAX_PATH];
        char caminho[MAX_PATH * 2];

        strncpy(prefixo, base_svg, sizeof(prefixo) - 1);
        prefixo[sizeof(prefixo) - 1] = '\0';

        char *ponto = strrchr(prefixo, '.');
        if (ponto)
            *ponto = '\0';

        snprintf(caminho, sizeof(caminho), "%s_frame_%03d.svg", prefixo, contador_mcs++);

        qry_svg_de_arvore(caminho, formas);
    }
}

/* Comando 'mc': altera as cores das formas selecionadas sem movê-las. */
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

/* Comando 'cm': clona as formas contidas na região especificada,
 * translada os clones pelo vetor (dx, dy) e os insere na árvore,
 * tornando-os a nova lista de selecionados. */
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
 * Comando 'find' e 'findrm': ordena as formas selecionadas de acordo
 * com o algoritmo e critério especificados, reposiciona os k primeiros
 * na fileira de destino e, opcionalmente, remove os demais.
 * Gera animação (frames SVG) se um nome base for fornecido.
 * Parâmetro eh_findrm: 0 para 'find', 1 para 'findrm'.
 * 
 * ALTERADO: adicionados ajuste de dw, pré‑cálculo do fundo estático,
 * ajuste de dimensões da tela e geração do snapshot inicial.
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

    /* Ajuste de espaçamento mínimo */
    if (dw < 10.0)
    {
        dw = 15.0;
        if (arq_txt)
            fprintf(arq_txt, "[*] dw ajustado para 15.0 (minimo para visualizacao)\n");
    }

    int n_sel = lista_tamanho(*selecionadas);
    if (n_sel == 0)
        return;

    void **vetor = (void **)malloc(n_sel * sizeof(void *));
    for (int i = 0; i < n_sel; i++)
        vetor[i] = lista_get(*selecionadas, i);

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

    /* Configura o contexto para geração de frames, se requisitado. */
    CtxFrames ctx_frames;
    ctx_frames.largura_tela = 0;
    ctx_frames.altura_tela = 0;
    ctx_frames.fundo_svg = NULL;
    ctx_frames.fundo_tam = 0;

    if (base_svg)
    {
        strncpy(ctx_frames.base, base_svg, MAX_PATH * 4 - 1);
        ctx_frames.base[MAX_PATH * 4 - 1] = '\0';
        char *ponto = strrchr(ctx_frames.base, '.');
        if (ponto) *ponto = '\0';

        /* Coleta todas as formas para calcular dimensões */
        Lista *todas = lista_criar();
        emOrdemArvore(formas, cb_coleta_lista, todas);
        svg_calcula_dimensoes(todas, &ctx_frames.largura_tela, &ctx_frames.altura_tela);

        /* BUGFIX: ajusta dimensões para incluir a fileira de destino */
        {
            double maior_largura_forma = 0.0, maior_altura_forma = 0.0;
            for (int idx = 0; idx < n_sel; idx++)
            {
                Forma *f = (Forma *)vetor[idx];
                double lf = geo_largura(f), af = geo_altura(f);
                if (lf > maior_largura_forma) maior_largura_forma = lf;
                if (af > maior_altura_forma) maior_altura_forma = af;
            }
            double fim_x = dest_x + (n_sel * dw) + maior_largura_forma + 20.0;
            double fim_y = dest_y + maior_altura_forma + 20.0;
            if (fim_x > ctx_frames.largura_tela) ctx_frames.largura_tela = fim_x;
            if (fim_y > ctx_frames.altura_tela) ctx_frames.altura_tela = fim_y;
        }

        /* Gera o fundo estático (formas não selecionadas) em memória */
        FILE *mem = open_memstream(&ctx_frames.fundo_svg, &ctx_frames.fundo_tam);
        if (mem)
        {
            int n_todas = lista_tamanho(todas);
            for (int idx = 0; idx < n_todas; idx++)
            {
                Forma *f = (Forma *)lista_get(todas, idx);
                int esta_no_vetor = 0;
                for (int k2 = 0; k2 < n_sel; k2++)
                {
                    if (vetor[k2] == f)
                    {
                        esta_no_vetor = 1;
                        break;
                    }
                }
                if (!esta_no_vetor)
                {
                    svg_desenha_forma(mem, f, ctx_frames.altura_tela);
                }
            }
            fclose(mem);
        }
        lista_destruir(todas);
    }

    ctx_frames.regiao = ultima_regiao;
    ctx_frames.contador = 1;
    ctx_frames.formas = formas;
    ctx_frames.dest_x = dest_x;
    ctx_frames.dest_y = dest_y;
    ctx_frames.dw = dw;

    FuncaoSnapshot snap = base_svg ? snapshot_frame : NULL;
    if (snap)
        snap(vetor, n_sel, -1, -1, &ctx_frames);

    /* Executa o algoritmo de ordenação escolhido. */
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

    /* Libera o buffer do fundo, se alocado */
    if (ctx_frames.fundo_svg)
        free(ctx_frames.fundo_svg);

    if (k > n_sel)
        k = n_sel;

    /* Registra no arquivo de texto os k primeiros elementos com seus
     * atributos relevantes. */
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

    /* Reposiciona os k primeiros na fileira de destino. */
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

    /* Se for 'findrm', remove os elementos restantes (índices > k). */
    if (eh_findrm)
    {
        for (int i = k; i < n_sel; i++)
        {
            Forma *f = (Forma *)vetor[i];
            removerArvore(formas, f);
            forma_destroi(f);
        }
    }

    /* Gera SVG final com quadrados vermelhos sobre os k menores e retângulo tracejado */
    if (base_svg)
        svg_de_arvore_com_quadrados(base_svg, formas, vetor, k, dest_x, dest_y, dw);

    /* Atualiza a lista de selecionados: se for 'find', mantém os k
     * primeiros; se for 'findrm', esvazia. */
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
 * PROCESSADOR PRINCIPAL DA QRV
 * ============================================================= */

/*
 * qry_processa_arquivo – Lê o arquivo de consultas (.qry) e executa
 * os comandos na ordem em que aparecem. Mantém uma lista de formas
 * selecionadas entre os comandos. Gera saídas de texto e SVG conforme
 * especificado.
 */
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