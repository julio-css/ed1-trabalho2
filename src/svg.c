#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svg.h"
#include "forma.h"
#include "lista.h"

/*
 * ============================================================
 * CONSTANTES DE ESTILO
 * ============================================================
 */

/** Margem em pixels ao redor de todas as formas no SVG. */
#define SVG_MARGEM 20.0

/** Raio do anel vermelho desenhado na âncora das formas selecionadas. */
#define SVG_RAIO_ANEL 6.0

/** Tamanho do "X" vermelho desenhado na âncora das formas removidas. */
#define SVG_TAMANHO_X 5.0

/*
 * ============================================================
 * FUNÇÕES AUXILIARES DE CONVERSÃO DE COORDENADAS
 * ============================================================
 */

/**
 * converte_y – translada y somando a margem.
 *
 * No SVG o y cresce para baixo. As coordenadas do .geo
 * já estão no sistema de tela (y=0 no topo), então
 * só precisamos empurrar pela margem.
 * O parâmetro 'altura' é mantido para compatibilidade futura.
 */
static double converte_y(double y, double altura)
{
    (void)altura; /* Evita warning de parâmetro não utilizado */
    return y + SVG_MARGEM;
}

/**
 * converte_x – translada x somando a margem.
 *
 * Garante que nenhuma forma fique colada na borda esquerda.
 */
static double converte_x(double x)
{
    return x + SVG_MARGEM;
}

/*
 * ============================================================
 * CÁLCULO DE DIMENSÕES
 * ============================================================
 */

/**
 * svg_calcula_dimensoes – percorre a lista e acha o maior x e y.
 *
 * Usa esses valores para definir o tamanho do canvas SVG.
 * Adiciona margem dos dois lados para não cortar as bordas.
 *
 * @param formas  Lista de formas.
 * @param largura Ponteiro para armazenar a largura calculada.
 * @param altura  Ponteiro para armazenar a altura calculada.
 */
void svg_calcula_dimensoes(Lista *formas, double *largura, double *altura)
{
    double max_x = 0.0, max_y = 0.0;
    int n = lista_tamanho(formas);

    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(formas, i);
        double x = forma_get_x(f);
        double y = forma_get_y(f);

        switch (forma_get_tipo(f))
        {
        case FORMA_CIRCULO:
        {
            double r = forma_get_raio(f);
            if (x + r > max_x)
                max_x = x + r;
            if (y + r > max_y)
                max_y = y + r;
            break;
        }
        case FORMA_RETANGULO:
        {
            double w = forma_get_largura(f);
            double h = forma_get_altura(f);
            if (x + w > max_x)
                max_x = x + w;
            if (y + h > max_y)
                max_y = y + h;
            break;
        }
        case FORMA_LINHA:
        {
            double x2 = forma_get_x2(f);
            double y2 = forma_get_y2(f);
            if (x > max_x)
                max_x = x;
            if (x2 > max_x)
                max_x = x2;
            if (y > max_y)
                max_y = y;
            if (y2 > max_y)
                max_y = y2;
            break;
        }
        case FORMA_TEXTO:
            if (x > max_x)
                max_x = x;
            if (y > max_y)
                max_y = y;
            break;
        case FORMA_POLIGONO:
        {
            int num_pts = forma_get_num_pontos(f);
            for (int k = 0; k < num_pts; k++)
            {
                double px = forma_get_ponto_x(f, k);
                double py = forma_get_ponto_y(f, k);
                if (px > max_x)
                    max_x = px;
                if (py > max_y)
                    max_y = py;
            }
            break;
        }
        default:
            break;
        }
    }

    *largura = max_x + (SVG_MARGEM * 2);
    *altura = max_y + (SVG_MARGEM * 2);
}

/*
 * ============================================================
 * ABERTURA E FECHAMENTO DO SVG
 * ============================================================
 */

/**
 * svg_abre – escreve o cabeçalho XML do SVG.
 *
 * Usa viewBox para o SVG se auto-escalar.
 *
 * @param arq     Ponteiro para o arquivo aberto.
 * @param largura Largura do canvas SVG.
 * @param altura  Altura do canvas SVG.
 */
void svg_abre(FILE *arq, double largura, double altura)
{
    fprintf(arq,
            "<svg xmlns=\"http://www.w3.org/2000/svg\""
            " viewBox=\"0 0 %.2f %.2f\""
            " width=\"100%%\" height=\"100%%\">\n",
            largura, altura);
}

/**
 * svg_abre_com_viewbox – escreve o cabeçalho XML do SVG
 * com viewBox personalizada para focar em uma área específica.
 *
 * Útil para SVGs finais que incluem a fileira de destino,
 * garantindo que toda a área seja visível.
 *
 * @param arq     Ponteiro para o arquivo aberto.
 * @param largura Largura da viewBox.
 * @param altura  Altura da viewBox.
 * @param view_x  Coordenada X do canto superior esquerdo da viewBox.
 * @param view_y  Coordenada Y do canto superior esquerdo da viewBox.
 */
void svg_abre_com_viewbox(FILE *arq, double largura, double altura,
                          double view_x, double view_y)
{
    fprintf(arq,
            "<svg xmlns=\"http://www.w3.org/2000/svg\""
            " viewBox=\"%.2f %.2f %.2f %.2f\""
            " width=\"100%%\" height=\"100%%\">\n",
            view_x, view_y, largura, altura);
}

/**
 * svg_fecha – escreve o fechamento do SVG.
 */
void svg_fecha(FILE *arq)
{
    fprintf(arq, "</svg>\n");
}

/*
 * ============================================================
 * DESENHO DE FORMAS
 * ============================================================
 */

/**
 * svg_desenha_forma – desenha uma forma no arquivo SVG.
 *
 * Cada tipo gera uma tag SVG diferente.
 * converte_x e converte_y aplicam a margem nas coordenadas.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param f      Ponteiro para a forma a ser desenhada.
 * @param altura Altura do canvas (usada para conversão Y).
 */
void svg_desenha_forma(FILE *arq, Forma *f, double altura)
{
    char *corb = forma_get_cor_borda(f);
    char *corp = forma_get_cor_preench(f);
    double x = forma_get_x(f);
    double y = forma_get_y(f);
    int id = forma_get_id(f);
    double sx = converte_x(x);

    switch (forma_get_tipo(f))
    {
    case FORMA_CIRCULO:
    {
        double cy = converte_y(y, altura);
        fprintf(arq,
                "  <circle id=\"%d\" cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\""
                " style=\"fill:%s;fill-opacity:0.5;stroke:%s\"/>\n",
                id, sx, cy, forma_get_raio(f), corp, corb);
        break;
    }
    case FORMA_RETANGULO:
    {
        double w = forma_get_largura(f);
        double h = forma_get_altura(f);
        double ry = converte_y(y, altura);
        fprintf(arq,
                "  <rect id=\"%d\" x=\"%.2f\" y=\"%.2f\""
                " width=\"%.2f\" height=\"%.2f\""
                " style=\"fill:%s;fill-opacity:0.5;stroke:%s\"/>\n",
                id, sx, ry, w, h, corp, corb);
        break;
    }
    case FORMA_LINHA:
    {
        fprintf(arq,
                "  <line id=\"%d\" x1=\"%.2f\" y1=\"%.2f\""
                " x2=\"%.2f\" y2=\"%.2f\""
                " style=\"stroke:%s;stroke-width:1\"/>\n",
                id,
                sx, converte_y(y, altura),
                converte_x(forma_get_x2(f)), converte_y(forma_get_y2(f), altura),
                corb);
        break;
    }
    case FORMA_TEXTO:
    {
        char *anchor = "start";
        if (forma_get_ancora(f) == 'm')
            anchor = "middle";
        else if (forma_get_ancora(f) == 'f')
            anchor = "end";
        fprintf(arq,
                "  <text id=\"%d\" x=\"%.2f\" y=\"%.2f\""
                " text-anchor=\"%s\""
                " style=\"font-size:12px;fill:%s;stroke:%s\">%s</text>\n",
                id, sx, converte_y(y, altura),
                anchor, corp, corb, forma_get_texto(f));
        break;
    }
    case FORMA_POLIGONO:
    {
        fprintf(arq, "  <polygon id=\"%d\" points=\"", id);
        int n_pts = forma_get_num_pontos(f);
        for (int k = 0; k < n_pts; k++)
        {
            double px = converte_x(forma_get_ponto_x(f, k));
            double py = converte_y(forma_get_ponto_y(f, k), altura);
            fprintf(arq, "%.2f,%.2f ", px, py);
        }
        fprintf(arq,
                "\" style=\"fill:%s;fill-opacity:0.8;stroke:%s\"/>\n",
                corp, corb);
        break;
    }
    default:
        break;
    }
}

/*
 * ============================================================
 * DESENHO DE LISTAS DE FORMAS
 * ============================================================
 */

/**
 * svg_desenha_lista_altura – desenha todas as formas reutilizando
 * uma altura já calculada.
 *
 * Evita recalcular as dimensões quando elas já são conhecidas
 * pelo chamador. Melhora a performance em chamadas repetidas.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param formas Lista de formas a serem desenhadas.
 * @param altura Altura do canvas (já calculada).
 */
void svg_desenha_lista_altura(FILE *arq, Lista *formas, double altura)
{
    int n = lista_tamanho(formas);
    for (int i = 0; i < n; i++)
        svg_desenha_forma(arq, (Forma *)lista_get(formas, i), altura);
}

/**
 * svg_desenha_lista – desenha todas as formas da lista.
 *
 * Calcula a altura internamente (chama svg_calcula_dimensoes).
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param formas Lista de formas a serem desenhadas.
 */
void svg_desenha_lista(FILE *arq, Lista *formas)
{
    double l, a;
    svg_calcula_dimensoes(formas, &l, &a);
    svg_desenha_lista_altura(arq, formas, a);
}

/*
 * ============================================================
 * GERAÇÃO DE ARQUIVO COMPLETO
 * ============================================================
 */

/**
 * svg_gera_arquivo – função principal do módulo.
 *
 * Abre o arquivo, calcula dimensões, desenha tudo e fecha.
 * Usa um buffer de 1MB para otimizar a escrita em disco.
 *
 * @param caminho Caminho do arquivo SVG a ser gerado.
 * @param formas  Lista de formas a serem desenhadas.
 * @return 1 se gerou com sucesso, 0 se não conseguiu abrir o arquivo.
 */
int svg_gera_arquivo(char *caminho, Lista *formas)
{
    FILE *arq = fopen(caminho, "w");
    if (!arq)
        return 0;

    /* Buffer de 1MB para melhorar performance de escrita */
    static char buffer[1024 * 1024];
    setvbuf(arq, buffer, _IOFBF, sizeof(buffer));

    double l, a;
    svg_calcula_dimensoes(formas, &l, &a);
    svg_abre(arq, l, a);
    svg_desenha_lista_altura(arq, formas, a);
    svg_fecha(arq);

    fclose(arq);
    return 1;
}

/*
 * ============================================================
 * DESENHO DE ELEMENTOS VISUAIS AUXILIARES
 * ============================================================
 */

/**
 * svg_desenha_selecao – desenha a região de seleção e marca as âncoras.
 *
 * Conforme o PDF:
 * - Região: retângulo com borda vermelha pontilhada
 * - Âncoras das formas selecionadas: anel vermelho pequeno
 *
 * O retângulo é desenhado com stroke-dasharray para ficar pontilhado.
 * O anel é um <circle> sem preenchimento com borda vermelha.
 *
 * @param arq          Ponteiro para o arquivo aberto.
 * @param x            Coordenada X do canto superior esquerdo da região.
 * @param y            Coordenada Y do canto superior esquerdo da região.
 * @param w            Largura da região.
 * @param h            Altura da região.
 * @param selecionadas Lista de formas selecionadas.
 * @param altura       Altura do canvas (usada para conversão Y).
 */
void svg_desenha_selecao(FILE *arq, double x, double y,
                         double w, double h,
                         Lista *selecionadas, double altura)
{
    /* Retângulo da região — borda vermelha pontilhada, sem preenchimento */
    fprintf(arq,
            "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\""
            " style=\"fill:none;stroke:red;stroke-width:1.5;"
            "stroke-dasharray:5,3\"/>\n",
            converte_x(x), converte_y(y, altura), w, h);

    /* Anel vermelho na âncora de cada forma selecionada */
    int n = lista_tamanho(selecionadas);
    for (int i = 0; i < n; i++)
    {
        Forma *f = (Forma *)lista_get(selecionadas, i);
        double ax = converte_x(forma_get_x(f));
        double ay = converte_y(forma_get_y(f), altura);

        fprintf(arq,
                "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\""
                " style=\"fill:none;stroke:red;stroke-width:1.5\"/>\n",
                ax, ay, SVG_RAIO_ANEL);
    }
}

/**
 * svg_desenha_x_remocao – desenha um "X" vermelho na âncora removida.
 *
 * O "X" é formado por duas linhas diagonais cruzadas.
 * Usado pelo comando 'dels' para marcar visualmente as formas que serão removidas.
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param ax     Coordenada X da âncora (sistema cartesiano).
 * @param ay     Coordenada Y da âncora (sistema cartesiano).
 * @param altura Altura do canvas (usada para conversão Y).
 */
void svg_desenha_x_remocao(FILE *arq, double ax, double ay, double altura)
{
    double sx = converte_x(ax);
    double sy = converte_y(ay, altura);
    double d = SVG_TAMANHO_X;

    /* Diagonal de cima-esquerda para baixo-direita */
    fprintf(arq,
            "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
            " style=\"stroke:red;stroke-width:2\"/>\n",
            sx - d, sy - d, sx + d, sy + d);

    /* Diagonal de cima-direita para baixo-esquerda */
    fprintf(arq,
            "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\""
            " style=\"stroke:red;stroke-width:2\"/>\n",
            sx + d, sy - d, sx - d, sy + d);
}

/**
 * svg_desenha_quadrado_selecao – quadrado vermelho na âncora do comando 'find'.
 *
 * Conforme o PDF: "marcar a âncora dos k menores com um pequeno quadrado vermelho".
 *
 * @param arq    Ponteiro para o arquivo aberto.
 * @param ax     Coordenada X da âncora (sistema cartesiano).
 * @param ay     Coordenada Y da âncora (sistema cartesiano).
 * @param altura Altura do canvas (usada para conversão Y).
 */
void svg_desenha_quadrado_selecao(FILE *arq, double ax, double ay, double altura)
{
    double sx = ax + SVG_MARGEM;
    double sy = ay + SVG_MARGEM;
    double d = 4.0; /* Metade do lado do quadrado */
    (void)altura;   /* Evita warning de parâmetro não utilizado */
    fprintf(arq,
            "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\""
            " style=\"fill:red;stroke:red\"/>\n",
            sx - d, sy - d, d * 2, d * 2);
}