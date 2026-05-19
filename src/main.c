#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* modulos do projeto */
#include "lista.h"
#include "fila.h"
#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "poligono.h"
#include "svg.h"

/* tamanho maximo pra caminhos de arquivo */
#define MAX_PATH      512

/* limite de poligonos simultaneos conforme o trabalho */
#define MAX_POLIGONOS 10

/*
 * processa_geo — le o arquivo .geo linha por linha
 * cada linha começa com um comando e seus parametros
 * cria a forma e insere na lista
 */
void processa_geo(FILE* arq_geo, Lista* formas) {
    char cmd[4]; 

    /* le um comando por vez ate acabar o arquivo */
    while (fscanf(arq_geo, "%3s", cmd) == 1) {
        if (strcmp(cmd, "c") == 0) {
            /* c id x y r cor_borda cor_preench */
            int id; double x, y, r; char corb[32], corp[32];
            fscanf(arq_geo, "%d %lf %lf %lf %31s %31s", &id, &x, &y, &r, corb, corp);
            Forma* f = circulo_cria(id, x, y, r, corb, corp);
            if (f != NULL) lista_inserir_fim(formas, f);

        } else if (strcmp(cmd, "r") == 0) {
            /* r id x y w h cor_borda cor_preench */
            int id; double x, y, w, h; char corb[32], corp[32];
            fscanf(arq_geo, "%d %lf %lf %lf %lf %31s %31s", &id, &x, &y, &w, &h, corb, corp);
            Forma* f = retangulo_cria(id, x, y, w, h, corb, corp);
            if (f != NULL) lista_inserir_fim(formas, f);

        } else if (strcmp(cmd, "l") == 0) {
            /* l id x1 y1 x2 y2 cor */
            int id; double x1, y1, x2, y2; char cor[32];
            fscanf(arq_geo, "%d %lf %lf %lf %lf %31s", &id, &x1, &y1, &x2, &y2, cor);
            Forma* f = linha_cria(id, x1, y1, x2, y2, cor);
            if (f != NULL) lista_inserir_fim(formas, f);

        } else if (strcmp(cmd, "t") == 0) {
            /* t id x y cor_borda cor_preench ancora texto_ate_fim_da_linha */
            int id; double x, y; char corb[32], corp[32], ancora; char conteudo[256];
            fscanf(arq_geo, "%d %lf %lf %31s %31s %c ", &id, &x, &y, corb, corp, &ancora);
            /* fgets le o texto ate o fim da linha incluindo espacos */
            fgets(conteudo, sizeof(conteudo), arq_geo);
            int len = strlen(conteudo);
            /* remove o \n do final */
            if (len > 0 && conteudo[len-1] == '\n') conteudo[len-1] = '\0';
            
            Forma* f = forma_cria_texto(id, x, y, corb, corp, ancora, conteudo);
            if (f != NULL) lista_inserir_fim(formas, f);

        } else if (strcmp(cmd, "ts") == 0) {
            /* ts muda estilo dos textos */
            char familia[32], peso[32], tamanho[32];
            fscanf(arq_geo, "%31s %31s %31s", familia, peso, tamanho);
        }
    }
}

/*
 * busca_por_id — percorre a lista procurando a forma com o id informado
 * retorna o ponteiro pra Forma ou NULL se nao encontar
 */
Forma* busca_por_id(Lista* formas, int id) {
    int n = lista_tamanho(formas);
    for (int i = 0; i < n; i++) {
        Forma* f = (Forma*) lista_get(formas, i);
        if (forma_get_id(f) == id) return f;
    }
    return NULL; 
}

/*
 * processa_qry — le o arquivo .qry e executa os comandos
 *
 * comandos:
 *   inp → insere ancora da figura no poligono
 *   rmp → remove ancora mais antiga do poligono
 *   clp → limpa todas as coordenadas do poligono
 *   pol → gera as linhas do poligono e insere na lista
 *   sel → seleciona figuras dentro de uma regiao
 *   dels → remove as figuras selecionadas
 *   mcs → move e recolore as figuras selecionadas
 */

void processa_qry(FILE* arq_qry, Lista* formas, Poligono** poligonos, FILE* arq_txt) {
    char cmd[8];
    /* lista temporaria das figuras selecionadas pelo sel */
    Lista* selecionadas = lista_criar();

    while (fscanf(arq_qry, "%7s", cmd) == 1) {

        if (strcmp(cmd, "inp") == 0) {
            /* inp p i — insere ancora da figura i no poligono p */
            int p, i;
            fscanf(arq_qry, "%d %d", &p, &i);
            if (p < 1 || p > MAX_POLIGONOS) continue;
            Forma* f = busca_por_id(formas, i);
            if (f == NULL) continue;
            
            /* pega a ancora — pra linha usa a extremidade de menor x */
            double ax = forma_get_x(f), ay = forma_get_y(f);
            if (forma_get_tipo(f) == FORMA_LINHA) {
                double x2 = forma_get_x2(f), y2 = forma_get_y2(f);
                if (x2 < ax || (x2 == ax && y2 < ay)) { ax = x2; ay = y2; }
            }
            pol_insere(poligonos[p-1], ax, ay, i);
            if (arq_txt) fprintf(arq_txt, "[*] inp %d %d -> (%.2f, %.2f)\n", p, i, ax, ay);

        } else if (strcmp(cmd, "rmp") == 0) {
            /* rmp p — remove a ancora mais antiga do poligono p */
            int p; fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS) continue;
            double ox, oy; int oid;
            if (pol_remove(poligonos[p-1], &ox, &oy, &oid))
                if (arq_txt) fprintf(arq_txt, "[*] rmp %d: removido pt de id %d\n", p, oid);

        } else if (strcmp(cmd, "clp") == 0) {
            /* clp p — limpa todas as coordenadas do poligono p */
            int p; fscanf(arq_qry, "%d", &p);
            if (p < 1 || p > MAX_POLIGONOS) continue;
            pol_limpa(poligonos[p-1]);
            if (arq_txt) fprintf(arq_txt, "[*] clp %d: poligono esvaziado\n", p);

        } else if (strcmp(cmd, "pol") == 0) {
            /* pol p id esp corb corp — gera linhas do poligono p com id inicial */
            int p, id_pol; double esp; char corb[32], corp[32];
            fscanf(arq_qry, "%d %d %lf %31s %31s", &p, &id_pol, &esp, corb, corp);
            
            /* precisa de pelo menos 3 pontos pra formar um poligono */
            if (p >= 1 && p <= MAX_POLIGONOS) {
                int n_pts = pol_tamanho(poligonos[p-1]);
                if (n_pts >= 3) {
                    double* px = (double*) malloc(n_pts * sizeof(double));
                    double* py = (double*) malloc(n_pts * sizeof(double));
                    int oid;
                    
                    /* copia as coordenadas do poligono pra arrays temporarios */
                    for (int k = 0; k < n_pts; k++) {
                        pol_get(poligonos[p-1], k, &px[k], &py[k], &oid);
                    }
                    
                    /* cria a forma do poligono e insere na lista */
                    Forma* f = forma_cria_poligono(id_pol, n_pts, px, py, corb, corp);
                    if (f) lista_inserir_fim(formas, f);
                    
                    free(px);
                    free(py);
                }
            }
            if (arq_txt) fprintf(arq_txt, "[*] pol %d: gerado com id %d\n", p, id_pol);

        } else if (strcmp(cmd, "sel") == 0) {
            /* sel x y w h — seleciona figuras cuja ancora esta dentro da regiao */
            double x, y, w, h;
            fscanf(arq_qry, "%lf %lf %lf %lf", &x, &y, &w, &h);
            /* descarta selecao anterior */
            lista_destruir(selecionadas);
            selecionadas = lista_criar();
            
            int n = lista_tamanho(formas);
            if (arq_txt) fprintf(arq_txt, "[*] sel %.1f %.1f %.1f %.1f\n", x, y, w, h);
            
            for (int i = 0; i < n; i++) {
                Forma* f = (Forma*) lista_get(formas, i);
                int selecionado = 0;
                
                /* poligono: verifica se algum vertice esta dentro da area */
                if (forma_get_tipo(f) == FORMA_POLIGONO) {
                    int n_pts = forma_get_num_pontos(f);
                    for(int k = 0; k < n_pts; k++) {
                        double px = forma_get_ponto_x(f, k);
                        double py = forma_get_ponto_y(f, k);
                        if (px >= x && px <= x+w && py >= y && py <= y+h) {
                            selecionado = 1;
                            break; /* achou um ponto dentro, seleciona o poligono inteiro */
                        }
                    }
                } else {
                    /* demais formas: verifica a ancora principal */
                    double fx = forma_get_x(f), fy = forma_get_y(f);
                    if (fx >= x && fx <= x+w && fy >= y && fy <= y+h) {
                        selecionado = 1;
                    }
                }

                if (selecionado) {
                    lista_inserir_fim(selecionadas, f);
                    if (arq_txt) fprintf(arq_txt, "   - id %d selecionado\n", forma_get_id(f));
                }
            }
        } else if (strcmp(cmd, "dels") == 0) {
            /* dels — remove da lista todas as formas selecionadas */
            int n = lista_tamanho(selecionadas);
            for (int i = 0; i < n; i++) {
                Forma* f = (Forma*) lista_get(selecionadas, i);
                if (arq_txt) fprintf(arq_txt, "[*] dels: removido id %d\n", forma_get_id(f));
                /* remove da lista principal e libera memoria */
                lista_remove(formas, f);
                forma_destroi(f);
            }
            lista_destruir(selecionadas);
            selecionadas = lista_criar();

        } else if (strcmp(cmd, "mcs") == 0) {
            /* mcs dx dy corb corp — translada e recolore as selecionadas */
            double dx, dy; char corb[32], corp[32];
            fscanf(arq_qry, "%lf %lf %31s %31s", &dx, &dy, corb, corp);
            int n = lista_tamanho(selecionadas);
            for (int i = 0; i < n; i++) {
                Forma* f = (Forma*) lista_get(selecionadas, i);
                
                /* soma o deslocamento na posicao atual */
                forma_set_x(f, forma_get_x(f) + dx);
                forma_set_y(f, forma_get_y(f) + dy);
                
                /* linha tem dois pontos — translada os dois */
                if (forma_get_tipo(f) == FORMA_LINHA) {
                    forma_set_x2(f, forma_get_x2(f) + dx);
                    forma_set_y2(f, forma_get_y2(f) + dy);
                }
                /* poligono tem varios pontos — translada todos */
                else if (forma_get_tipo(f) == FORMA_POLIGONO) {
                    int n_pts = forma_get_num_pontos(f);
                    for(int j = 0; j < n_pts; j++) {
                        forma_set_ponto_x(f, j, forma_get_ponto_x(f, j) + dx);
                        forma_set_ponto_y(f, j, forma_get_ponto_y(f, j) + dy);
                    }
                }
                forma_set_cor_borda(f, corb);
                forma_set_cor_preench(f, corp);
            }
            if (arq_txt) fprintf(arq_txt, "[*] mcs: transladadas %d formas\n", n);
        }
    }
    lista_destruir(selecionadas);
}

/*
 * main — ponto de entrada do programa ted
 *
 * parametros:
 *   -e path → diretorio base de entrada (opcional, padrao: .)
 *   -f arq  → arquivo .geo com as formas (obrigatorio)
 *   -o path → diretorio de saida (obrigatorio)
 *   -q arq  → arquivo .qry com as consultas (opcional)
 */

int main(int argc, char* argv[]) {
    char dir_entrada[MAX_PATH] = "."; 
    char dir_saida[MAX_PATH]   = "";
    char nome_geo[MAX_PATH]    = "";
    char nome_qry[MAX_PATH]    = "";

    /* processa os argumentos da linha de comando */
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-e") == 0 && i+1 < argc) strncpy(dir_entrada, argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-f") == 0 && i+1 < argc) strncpy(nome_geo,    argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-o") == 0 && i+1 < argc) strncpy(dir_saida,   argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-q") == 0 && i+1 < argc) strncpy(nome_qry,    argv[++i], MAX_PATH-1);
    }

    /* verifica os parametros obrigatorios */
    if (strlen(nome_geo) == 0 || strlen(dir_saida) == 0) {
        fprintf(stderr, "Uso: ted [-e path] -f arq.geo -o dir [-q arq.qry]\n");
        return 1;
    }

    /* monta o caminho completo do .geo e abre */
    char caminho_geo[MAX_PATH * 2];
    snprintf(caminho_geo, sizeof(caminho_geo), "%s/%s", dir_entrada, nome_geo);
    FILE* arq_geo = fopen(caminho_geo, "r");
    if (!arq_geo) { fprintf(stderr, "Erro ao abrir %s\n", caminho_geo); return 1; }

    /* cria a lista de formas e os 10 poligonos */
    Lista* formas = lista_criar();
    Poligono* poligonos[MAX_POLIGONOS];
    for (int i = 0; i < MAX_POLIGONOS; i++) poligonos[i] = pol_cria();

    /* processa o .geo — preenche a lista de formas */
    processa_geo(arq_geo, formas);
    fclose(arq_geo);

    /* gera o svg inicial so com as formas do .geo */
    char base_geo[MAX_PATH], svg_path[MAX_PATH * 3];
    strncpy(base_geo, nome_geo, MAX_PATH-1);
    char* pt = strrchr(base_geo, '.'); if (pt) *pt = '\0';
    snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", dir_saida, base_geo);
    svg_gera_arquivo(svg_path, formas);

    /* processa o .qry se foi informado */
    if (strlen(nome_qry) > 0) {
        char caminho_qry[MAX_PATH * 2];
        snprintf(caminho_qry, sizeof(caminho_qry), "%s/%s", dir_entrada, nome_qry);
        FILE* arq_qry = fopen(caminho_qry, "r");
        if (arq_qry) {
            /* monta nomes dos arquivos de saida: base_geo-base_qry.txt e .svg */
            char base_qry[MAX_PATH], caminho_txt[MAX_PATH * 4], caminho_svg_fin[MAX_PATH * 4];
            strncpy(base_qry, nome_qry, MAX_PATH-1);
            pt = strrchr(base_qry, '.'); if (pt) *pt = '\0';

            snprintf(caminho_txt, sizeof(caminho_txt), "%s/%s-%s.txt", dir_saida, base_geo, base_qry);
            FILE* arq_txt = fopen(caminho_txt, "w");
            
            /* executa os comandos do .qry */
            processa_qry(arq_qry, formas, poligonos, arq_txt);
            
            if (arq_txt) fclose(arq_txt);
            fclose(arq_qry);

            /* gera o svg final com o estado apos o .qry */
            snprintf(caminho_svg_fin, sizeof(caminho_svg_fin), "%s/%s-%s.svg", dir_saida, base_geo, base_qry);
            svg_gera_arquivo(caminho_svg_fin, formas);
        }
    }

    /* libera todos os poligonos */
    for (int i = 0; i < MAX_POLIGONOS; i++) pol_destroi(poligonos[i]);

    /* libera todas as formas e a lista */
    int n = lista_tamanho(formas);
    for (int i = 0; i < n; i++) forma_destroi((Forma*) lista_get(formas, i));
    lista_destruir(formas);

    return 0;
}