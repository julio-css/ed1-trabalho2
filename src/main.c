#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "arvore.h"
#include "forma.h"
#include "poligono.h"
#include "geo.h"
#include "qry.h"

#define MAX_PATH 512

/*
 * ============================================================
 * FUNÇÃO PRINCIPAL
 * ============================================================
 */

/**
 * main – ponto de entrada do programa TED (Trabalho de Estrutura de Dados).
 *
 * Processa os argumentos da linha de comando, lê o arquivo .geo,
 * gera o SVG inicial, processa o arquivo .qry (se fornecido),
 * e gera o SVG final.
 *
 * Sintaxe:
 *   ted [-e path] -f arq.geo -o dir [-q arq.qry]
 *
 * -e path : diretório de entrada (padrão: .)
 * -f arq  : arquivo .geo (obrigatório)
 * -o dir  : diretório de saída (obrigatório)
 * -q arq  : arquivo .qry (opcional)
 */
int main(int argc, char *argv[])
{
    char dir_entrada[MAX_PATH] = ".";
    char dir_saida[MAX_PATH] = "";
    char nome_geo[MAX_PATH] = "";
    char nome_qry[MAX_PATH] = "";

    /*
     * ============================================================
     * LEITURA DOS ARGUMENTOS DA LINHA DE COMANDO
     * ============================================================
     */

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc)
            strncpy(dir_entrada, argv[++i], MAX_PATH - 1);
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
            strncpy(nome_geo, argv[++i], MAX_PATH - 1);
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            strncpy(dir_saida, argv[++i], MAX_PATH - 1);
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
            strncpy(nome_qry, argv[++i], MAX_PATH - 1);
    }

    /* Validação: arquivo .geo e diretório de saída são obrigatórios */
    if (strlen(nome_geo) == 0 || strlen(dir_saida) == 0)
    {
        fprintf(stderr, "uso: ted [-e path] -f arq.geo -o dir [-q arq.qry]\n");
        return 1;
    }

    /*
     * ============================================================
     * CRIAÇÃO DO DIRETÓRIO DE SAÍDA
     * ============================================================
     */

    /* Cria o diretório de saída se não existir (0755 = rwxr-xr-x) */
    mkdir(dir_saida, 0755);

    /*
     * ============================================================
     * LEITURA DO ARQUIVO .geo
     * ============================================================
     */

    char caminho_geo[MAX_PATH * 2];
    snprintf(caminho_geo, sizeof(caminho_geo), "%s/%s", dir_entrada, nome_geo);
    FILE *arq_geo = fopen(caminho_geo, "r");
    if (!arq_geo)
    {
        fprintf(stderr, "erro ao abrir %s\n", caminho_geo);
        return 1;
    }

    /* Cria a árvore usando o comparador padrão (ordem default) */
    Arvore formas = criarArvore(geo_comparar);

    /* Inicializa os polígonos (máximo de 10, conforme especificação) */
    Poligono *poligonos[MAX_POLIGONOS];
    for (int i = 0; i < MAX_POLIGONOS; i++)
        poligonos[i] = pol_cria();

    /* Lê o arquivo .geo e insere as formas na árvore */
    geo_processa_arquivo(arq_geo, formas);
    fclose(arq_geo);

    /*
     * ============================================================
     * GERAÇÃO DO SVG INICIAL
     * ============================================================
     */

    /* Remove a extensão .geo para usar como base nos nomes dos arquivos */
    char base_geo[MAX_PATH], svg_inicial[MAX_PATH * 3];
    strncpy(base_geo, nome_geo, MAX_PATH - 1);
    char *pt = strrchr(base_geo, '.');
    if (pt)
        *pt = '\0';

    /* Gera o SVG com todas as formas do arquivo .geo */
    snprintf(svg_inicial, sizeof(svg_inicial), "%s/%s.svg", dir_saida, base_geo);
    qry_svg_de_arvore(svg_inicial, formas);

    /*
     * ============================================================
     * PROCESSAMENTO DO ARQUIVO .qry (se fornecido)
     * ============================================================
     */

    if (strlen(nome_qry) > 0)
    {
        char caminho_qry[MAX_PATH * 2];
        snprintf(caminho_qry, sizeof(caminho_qry), "%s/%s", dir_entrada, nome_qry);
        FILE *arq_qry = fopen(caminho_qry, "r");

        if (arq_qry)
        {
            /*
             * Monta o nome base do .qry sem extensão e sem prefixo de subpasta.
             * Exemplo: "t1/polig-1.qry" vira "polig-1"
             */
            char base_qry[MAX_PATH];
            strncpy(base_qry, nome_qry, MAX_PATH - 1);
            pt = strrchr(base_qry, '.');
            if (pt)
                *pt = '\0';
            char *nome_base = strrchr(base_qry, '/');
            nome_base = nome_base ? nome_base + 1 : base_qry;

            char caminho_txt[MAX_PATH * 4];
            char caminho_svg[MAX_PATH * 4];

            /*
             * Nomes dos arquivos de saída:
             * - Texto: <base_geo>-<nome_base>.txt
             * - SVG final: <base_geo>-<nome_base>.svg
             */
            snprintf(caminho_txt, sizeof(caminho_txt),
                     "%s/%s-%s.txt", dir_saida, base_geo, nome_base);
            snprintf(caminho_svg, sizeof(caminho_svg),
                     "%s/%s-%s.svg", dir_saida, base_geo, nome_base);

            /* Abre o arquivo de texto para saída */
            FILE *arq_txt = fopen(caminho_txt, "w");

            /*
             * Processa os comandos do arquivo .qry.
             * Esta função pode gerar múltiplos SVGs intermediários
             * (animação da ordenação) e também o SVG final.
             */
            qry_processa_arquivo(arq_qry, formas, poligonos, arq_txt, caminho_svg);

            if (arq_txt)
                fclose(arq_txt);
            fclose(arq_qry);

            /*
             * Gera o SVG final com o estado da árvore após todos
             * os comandos do .qry serem executados.
             * Isso garante que o SVG final reflita exatamente o estado
             * final (formas reposicionadas, removidas, etc.).
             */
            qry_svg_de_arvore(caminho_svg, formas);
        }
    }

    /*
     * ============================================================
     * LIBERAÇÃO DE MEMÓRIA
     * ============================================================
     */

    /* Destroi todos os polígonos */
    for (int i = 0; i < MAX_POLIGONOS; i++)
        pol_destroi(poligonos[i]);

    /*
     * Destroi a árvore e todas as formas.
     * A função 'forma_destroi' libera a memória de cada forma,
     * incluindo os arrays dinâmicos de polígonos.
     */
    destruirArvore(formas, (FuncaoDestruir)forma_destroi);

    return 0;
}