#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arvore.h"
#include "forma.h"
#include "poligono.h"
#include "geo.h"
#include "qry.h"

#define MAX_PATH      512

int main(int argc, char* argv[]) {
    char dir_entrada[MAX_PATH] = ".";
    char dir_saida[MAX_PATH]   = "";
    char nome_geo[MAX_PATH]    = "";
    char nome_qry[MAX_PATH]    = "";

    /* le os argumentos da linha de comando */
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-e") == 0 && i+1 < argc)
            strncpy(dir_entrada, argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-f") == 0 && i+1 < argc)
            strncpy(nome_geo,    argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-o") == 0 && i+1 < argc)
            strncpy(dir_saida,   argv[++i], MAX_PATH-1);
        else if (strcmp(argv[i], "-q") == 0 && i+1 < argc)
            strncpy(nome_qry,    argv[++i], MAX_PATH-1);
    }

    if (strlen(nome_geo) == 0 || strlen(dir_saida) == 0) {
        fprintf(stderr, "uso: ted [-e path] -f arq.geo -o dir [-q arq.qry]\n");
        return 1;
    }

    /* abre o .geo e popula a arvore */
    char caminho_geo[MAX_PATH * 2];
    snprintf(caminho_geo, sizeof(caminho_geo), "%s/%s", dir_entrada, nome_geo);
    FILE* arq_geo = fopen(caminho_geo, "r");
    if (!arq_geo) {
        fprintf(stderr, "erro ao abrir %s\n", caminho_geo);
        return 1;
    }

    Arvore formas = criarArvore(geo_comparar);

    Poligono* poligonos[MAX_POLIGONOS];
    for (int i = 0; i < MAX_POLIGONOS; i++) poligonos[i] = pol_cria();

    geo_processa_arquivo(arq_geo, formas);
    fclose(arq_geo);

    /* gera o svg inicial so com as formas do .geo */
    char base_geo[MAX_PATH], svg_inicial[MAX_PATH * 3];
    strncpy(base_geo, nome_geo, MAX_PATH-1);
    char* pt = strrchr(base_geo, '.'); if (pt) *pt = '\0';
    snprintf(svg_inicial, sizeof(svg_inicial), "%s/%s.svg", dir_saida, base_geo);
    qry_svg_de_arvore(svg_inicial, formas);

    /* processa o .qry se foi informado */
    if (strlen(nome_qry) > 0) {
        char caminho_qry[MAX_PATH * 2];
        snprintf(caminho_qry, sizeof(caminho_qry), "%s/%s", dir_entrada, nome_qry);
        FILE* arq_qry = fopen(caminho_qry, "r");
        if (arq_qry) {
            /* monta o nome base do qry sem extensao e sem prefixo de subpasta
             * ex: "t1/polig-1.qry" vira "polig-1" */
            char base_qry[MAX_PATH];
            strncpy(base_qry, nome_qry, MAX_PATH-1);
            pt = strrchr(base_qry, '.'); if (pt) *pt = '\0';
            char* nome_base = strrchr(base_qry, '/');
            nome_base = nome_base ? nome_base + 1 : base_qry;

            char caminho_txt[MAX_PATH * 4];
            char caminho_svg[MAX_PATH * 4];
            snprintf(caminho_txt, sizeof(caminho_txt),
                     "%s/%s-%s.txt", dir_saida, base_geo, nome_base);
            snprintf(caminho_svg, sizeof(caminho_svg),
                     "%s/%s-%s.svg", dir_saida, base_geo, nome_base);

            FILE* arq_txt = fopen(caminho_txt, "w");
            qry_processa_arquivo(arq_qry, formas, poligonos, arq_txt, caminho_svg);
            if (arq_txt) fclose(arq_txt);
            fclose(arq_qry);

            /* svg final com o estado apos todos os comandos do .qry */
            qry_svg_de_arvore(caminho_svg, formas);
        }
    }

    /* libera tudo */
    for (int i = 0; i < MAX_POLIGONOS; i++) pol_destroi(poligonos[i]);
    destruirArvore(formas, (FuncaoDestruir) forma_destroi);

    return 0;
}