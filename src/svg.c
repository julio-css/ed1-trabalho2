#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svg.h"
#include "forma.h"
#include "lista.h"

/* margem em pixels ao redor de todas as formas */
#define SVG_MARGEM 20.0

/* raio do anel vermelho desenhado na ancora das formas selecionadas */
#define SVG_RAIO_ANEL 6.0

/* tamanho do x vermelho desenhado na ancora das formas removidas */
#define SVG_TAMANHO_X 5.0

/*
 * converte_y — translada y somando a margem
 * no SVG o y cresce pra baixo, mas aqui as coordenadas
 * do .geo ja estao no sistema de tela (y=0 no topo)
 * entao so precisamos empurrar pra baixo pela margem
 */
static double converte_y(double y, double altura) {
    return y + SVG_MARGEM;
}

/*
 * converte_x — translada x somando a margem
 * garante que nenhuma forma fique colada na borda esquerda
 */
static double converte_x(double x) {
    return x + SVG_MARGEM;
}

/*
 * svg_calcula_dimensoes — percorre a lista e acha o maior x e y
 * usa esses valores pra definir o tamanho do canvas SVG
 * adiciona margem dos dois lados pra nao cortar as bordas
 */
void svg_calcula_dimensoes(Lista* formas, double* largura, double* altura) {
    double max_x = 0.0, max_y = 0.0;
    int n = lista_tamanho(formas);

    for (int i = 0; i < n; i++) {
        Forma* f = (Forma*) lista_get(formas, i);
        double x = forma_get_x(f);
        double y = forma_get_y(f);
        
        switch (forma_get_tipo(f)) {

            case FORMA_CIRCULO: {
                /* circulo ocupa ate x+r e y+r */
                double r = forma_get_raio(f);
                if (x + r > max_x) max_x = x + r;
                if (y + r > max_y) max_y = y + r;
                break;
            }

            case FORMA_RETANGULO: {
                /* retangulo ocupa ate x+w e y+h */
                double w = forma_get_largura(f);
                double h = forma_get_altura(f);
                if (x + w > max_x) max_x = x + w;
                if (y + h > max_y) max_y = y + h;
                break;
            }

            case FORMA_LINHA: {
                /* linha tem dois pontos — verifica os dois extremos */
                double x2 = forma_get_x2(f);
                double y2 = forma_get_y2(f);
                if (x  > max_x) max_x = x; 
                if (x2 > max_x) max_x = x2;
                if (y  > max_y) max_y = y; 
                if (y2 > max_y) max_y = y2;
                break;
            }

            case FORMA_TEXTO:
                /* texto — usa a ancora como referencia de tamanho */
                if (x > max_x) max_x = x;
                if (y > max_y) max_y = y;
                break;

            case FORMA_POLIGONO: {
                /* poligono tem varios vertices — verifica todos */
                int num_pts = forma_get_num_pontos(f);
                for (int k = 0; k < num_pts; k++) {
                    double px = forma_get_ponto_x(f, k);
                    double py = forma_get_ponto_y(f, k);
                    if (px > max_x) max_x = px;
                    if (py > max_y) max_y = py;
                }
                break;
            }
        }
    }

    /* dimensao final = maior coordenada + margem dos dois lados */
    *largura = max_x + (SVG_MARGEM * 2);
    *altura  = max_y + (SVG_MARGEM * 2);
}

/*
 * svg_abre — escreve o cabecalho XML do SVG
 * usa viewBox pra o SVG se auto-escalar
 * fixa o tamanho visual em 800x800 independente do conteudo
 */
void svg_abre(FILE* arq, double largura, double altura) {
    fprintf(arq,
        "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " viewBox=\"0 0 %.2f %.2f\""
        " width=\"800\" height=\"800\">\n",
        largura, altura);
}

/*
 * svg_fecha — escreve o fechamento do SVG
 */
void svg_fecha(FILE* arq) {
    fprintf(arq, "</svg>\n");
}

/*
 * svg_desenha_forma — desenha uma forma no arquivo SVG
 * cada tipo de forma gera uma tag SVG diferente
 * converte_x e converte_y aplicam a margem nas coordenadas
 */
void svg_desenha_forma(FILE* arq, Forma* f, double altura) {
    char* corb = forma_get_cor_borda(f);
    char* corp = forma_get_cor_preench(f);
    double x   = forma_get_x(f);
    double y   = forma_get_y(f);
    int id     = forma_get_id(f);

    /* aplica a margem no x — vale pra todos os tipos */
    double sx  = converte_x(x);

    switch (forma_get_tipo(f)) {

        case FORMA_CIRCULO: {
            /* <circle cx cy r style> */
            double cy = converte_y(y, altura);
            fprintf(arq,
                "  <circle id=\"%d\" cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\""
                " style=\"fill:%s;fill-opacity:0.5;stroke:%s\"/>\n",
                id, sx, cy, forma_get_raio(f), corp, corb);
            break;
        }

        case FORMA_RETANGULO: {
            /* <rect x y width height style> */
            double w  = forma_get_largura(f);
            double h  = forma_get_altura(f);
            double ry = converte_y(y, altura);
            fprintf(arq,
                "  <rect id=\"%d\" x=\"%.2f\" y=\"%.2f\""
                " width=\"%.2f\" height=\"%.2f\""
                " style=\"fill:%s;fill-opacity:0.5;stroke:%s\"/>\n",
                id, sx, ry, w, h, corp, corb);
            break;
        }

        case FORMA_LINHA: {
            /* <line x1 y1 x2 y2 style> — converte os dois pontos */
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

        case FORMA_TEXTO: {
            /* <text x y text-anchor style>conteudo</text> */
            /* ancora define o alinhamento: i=start m=middle f=end */
            char* anchor = "start";
            if      (forma_get_ancora(f) == 'm') anchor = "middle";
            else if (forma_get_ancora(f) == 'f') anchor = "end";
            fprintf(arq,
                "  <text id=\"%d\" x=\"%.2f\" y=\"%.2f\""
                " text-anchor=\"%s\""
                " style=\"font-size:12px;fill:%s;stroke:%s\">%s</text>\n",
                id, sx, converte_y(y, altura),
                anchor, corp, corb, forma_get_texto(f));
            break;
        }

        case FORMA_POLIGONO: {
            /* <polygon points="x1,y1 x2,y2 ..."> */
            /* escreve todos os vertices convertidos */
            fprintf(arq, "  <polygon id=\"%d\" points=\"", id);
            int n_pts = forma_get_num_pontos(f);
            for (int k = 0; k < n_pts; k++) {
                double px = converte_x(forma_get_ponto_x(f, k));
                double py = converte_y(forma_get_ponto_y(f, k), altura);
                fprintf(arq, "%.2f,%.2f ", px, py);
            }
            fprintf(arq,
                "\" style=\"fill:%s;fill-opacity:0.8;stroke:%s\"/>\n",
                corp, corb);
            break;
        }
    }
}

/*
 * svg_desenha_lista — desenha todas as formas da lista
 * calcula as dimensoes uma vez e passa pra cada forma
 */
void svg_desenha_lista(FILE* arq, Lista* formas) {
    double l, a;
    svg_calcula_dimensoes(formas, &l, &a);
    for (int i = 0; i < lista_tamanho(formas); i++)
        svg_desenha_forma(arq, (Forma*) lista_get(formas, i), a);
}

/*
 * svg_gera_arquivo — funcao principal do modulo
 * abre o arquivo, calcula dimensoes, desenha tudo e fecha
 * retorna 1 se gerou com sucesso, 0 se nao conseguiu abrir
 */
int svg_gera_arquivo(char* caminho, Lista* formas) {
    FILE* arq = fopen(caminho, "w");
    if (!arq) return 0;

    double l, a;
    svg_calcula_dimensoes(formas, &l, &a);

    svg_abre(arq, l, a);
    svg_desenha_lista(arq, formas);
    svg_fecha(arq);

    fclose(arq);
    return 1;
}