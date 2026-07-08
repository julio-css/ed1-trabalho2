#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "geo.h"
#include "forma.h"
#include "arvore.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "poligono.h"

/*
 * ============================================================
 * CÁLCULOS GEOMÉTRICOS
 * ============================================================
 */

/**
 * geo_area – calcula a área da forma conforme especificação do trabalho.
 *
 * Regras:
 * - Círculo   : π * r²
 * - Retângulo : w * h
 * - Linha     : 1.5 * comprimento
 * - Texto     : 10.0 * número de caracteres
 * - Polígono  : 0 (não definido na especificação)
 */
double geo_area(Forma *f)
{
    switch (forma_get_tipo(f))
    {
    case FORMA_CIRCULO:
    {
        double r = forma_get_raio(f);
        return 3.141592653589793 * r * r;
    }
    case FORMA_RETANGULO:
        return forma_get_largura(f) * forma_get_altura(f);
    case FORMA_LINHA:
    {
        double dx = forma_get_x2(f) - forma_get_x(f);
        double dy = forma_get_y2(f) - forma_get_y(f);
        return 1.5 * sqrt(dx * dx + dy * dy);
    }
    case FORMA_TEXTO:
        return 10.0 * strlen(forma_get_texto(f));
    default:
        return 0.0;
    }
}

/**
 * geo_largura – calcula a largura lógica da forma.
 *
 * Regras:
 * - Círculo   : 2 * r
 * - Retângulo : w
 * - Linha     : |x2 - x1|
 * - Texto     : 1.0 * número de caracteres
 * - Polígono  : 0
 */
double geo_largura(Forma *f)
{
    switch (forma_get_tipo(f))
    {
    case FORMA_CIRCULO:
        return 2.0 * forma_get_raio(f);
    case FORMA_RETANGULO:
        return forma_get_largura(f);
    case FORMA_LINHA:
        return fabs(forma_get_x2(f) - forma_get_x(f));
    case FORMA_TEXTO:
        return 1.0 * strlen(forma_get_texto(f));
    default:
        return 0.0;
    }
}

/**
 * geo_altura – calcula a altura lógica da forma.
 *
 * Regras:
 * - Círculo   : 2 * r
 * - Retângulo : h
 * - Linha     : 1.5 (fixo, conforme especificação)
 * - Texto     : 10.0 (fixo, conforme especificação)
 * - Polígono  : 0
 */
double geo_altura(Forma *f)
{
    switch (forma_get_tipo(f))
    {
    case FORMA_CIRCULO:
        return 2.0 * forma_get_raio(f);
    case FORMA_RETANGULO:
        return forma_get_altura(f);
    case FORMA_LINHA:
        return 1.5;
    case FORMA_TEXTO:
        return 10.0;
    default:
        return 0.0;
    }
}

/*
 * ============================================================
 * COMPARADORES PARA ORDENAÇÃO
 * ============================================================
 */

/**
 * geo_comparar – comparador padrão (ordem default).
 *
 * Chave primária: y crescente.
 * Chave secundária: x crescente.
 * Chave terciária: área crescente.
 * Desempate final: ID (garante unicidade).
 *
 * A inclusão do ID como último critério é importante para
 * distinguir elementos com todas as chaves idênticas,
 * permitindo operações corretas de remoção na árvore.
 *
 * Usa tolerância de 1e-9 para comparações de ponto flutuante,
 * evitando problemas com precisão numérica.
 */
int geo_comparar(void *e1, void *e2)
{
    Forma *f1 = (Forma *)e1;
    Forma *f2 = (Forma *)e2;

    /* Primeiro critério: y */
    double dy = forma_get_y(f1) - forma_get_y(f2);
    if (dy < -1e-9)
        return -1;
    if (dy > 1e-9)
        return 1;

    /* Segundo critério: x */
    double dx = forma_get_x(f1) - forma_get_x(f2);
    if (dx < -1e-9)
        return -1;
    if (dx > 1e-9)
        return 1;

    /* Terceiro critério: área */
    double da = geo_area(f1) - geo_area(f2);
    if (da < -1e-9)
        return -1;
    if (da > 1e-9)
        return 1;

    /* Desempate final por ID (garante unicidade) */
    return forma_get_id(f1) - forma_get_id(f2);
}

/**
 * geo_comparar_area – comparador para ordenação por área crescente.
 *
 * Em caso de empate, usa o comparador padrão como desempate.
 */
int geo_comparar_area(void *e1, void *e2)
{
    Forma *f1 = (Forma *)e1;
    Forma *f2 = (Forma *)e2;

    double da = geo_area(f1) - geo_area(f2);
    if (da < -1e-9)
        return -1;
    if (da > 1e-9)
        return 1;

    return geo_comparar(e1, e2);
}

/**
 * geo_comparar_largura – comparador para ordenação por largura crescente.
 */
int geo_comparar_largura(void *e1, void *e2)
{
    Forma *f1 = (Forma *)e1;
    Forma *f2 = (Forma *)e2;

    double dw = geo_largura(f1) - geo_largura(f2);
    if (dw < -1e-9)
        return -1;
    if (dw > 1e-9)
        return 1;

    return geo_comparar(e1, e2);
}

/**
 * geo_comparar_altura – comparador para ordenação por altura crescente.
 */
int geo_comparar_altura(void *e1, void *e2)
{
    Forma *f1 = (Forma *)e1;
    Forma *f2 = (Forma *)e2;

    double dh = geo_altura(f1) - geo_altura(f2);
    if (dh < -1e-9)
        return -1;
    if (dh > 1e-9)
        return 1;

    return geo_comparar(e1, e2);
}

/**
 * geo_comparar_cor – comparador para ordenação por cor.
 *
 * Para linhas, usa a cor da borda (conforme especificação).
 * Para outras formas, usa a cor de preenchimento.
 * Ordenação alfabética (strcmp).
 * Desempate pelo comparador padrão.
 */
int geo_comparar_cor(void *e1, void *e2)
{
    Forma *f1 = (Forma *)e1;
    Forma *f2 = (Forma *)e2;

    char *c1 = (forma_get_tipo(f1) == FORMA_LINHA)
                   ? forma_get_cor_borda(f1)
                   : forma_get_cor_preench(f1);
    char *c2 = (forma_get_tipo(f2) == FORMA_LINHA)
                   ? forma_get_cor_borda(f2)
                   : forma_get_cor_preench(f2);

    int cmp = strcmp(c1, c2);
    if (cmp != 0)
        return cmp;

    return geo_comparar(e1, e2);
}

/*
 * ============================================================
 * UTILITÁRIOS
 * ============================================================
 */

/**
 * geo_get_id – extrai o ID de uma forma (callback para busca por ID).
 *
 * Esta função é usada como callback em buscarPorIdArvore,
 * permitindo que a árvore busque elementos por ID sem conhecer
 * a estrutura específica de Forma.
 */
int geo_get_id(void *elemento)
{
    return forma_get_id((Forma *)elemento);
}

/*
 * ============================================================
 * LEITURA DO ARQUIVO .geo
 * ============================================================
 */

/**
 * geo_processa_arquivo – lê o arquivo .geo e insere formas na árvore.
 *
 * Suporta os comandos do formato .geo:
 * - c   : círculo
 * - r   : retângulo
 * - l   : linha
 * - t   : texto
 * - ts  : estilo de texto (ignorado, mas consumido)
 *
 * Linhas começando com '#' são ignoradas (comentários).
 *
 * @param arq_geo Arquivo .geo aberto para leitura.
 * @param formas  Árvore onde as formas serão inseridas.
 */
void geo_processa_arquivo(FILE *arq_geo, Arvore formas)
{
    char cmd[4];

    while (fscanf(arq_geo, "%3s", cmd) == 1)
    {
        if (strcmp(cmd, "c") == 0)
        {
            /* Círculo: c id x y r cor_borda cor_preench */
            int id;
            double x, y, r;
            char corb[32], corp[32];
            fscanf(arq_geo, "%d %lf %lf %lf %31s %31s",
                   &id, &x, &y, &r, corb, corp);
            Forma *f = circulo_cria(id, x, y, r, corb, corp);
            if (f)
                inserirArvore(formas, f);
        }
        else if (strcmp(cmd, "r") == 0)
        {
            /* Retângulo: r id x y w h cor_borda cor_preench */
            int id;
            double x, y, w, h;
            char corb[32], corp[32];
            fscanf(arq_geo, "%d %lf %lf %lf %lf %31s %31s",
                   &id, &x, &y, &w, &h, corb, corp);
            Forma *f = retangulo_cria(id, x, y, w, h, corb, corp);
            if (f)
                inserirArvore(formas, f);
        }
        else if (strcmp(cmd, "l") == 0)
        {
            /* Linha: l id x1 y1 x2 y2 cor */
            int id;
            double x1, y1, x2, y2;
            char cor[32];
            fscanf(arq_geo, "%d %lf %lf %lf %lf %31s",
                   &id, &x1, &y1, &x2, &y2, cor);
            Forma *f = linha_cria(id, x1, y1, x2, y2, cor);
            if (f)
                inserirArvore(formas, f);
        }
        else if (strcmp(cmd, "t") == 0)
        {
            /* Texto: t id x y cor_borda cor_preench ancora texto */
            int id;
            double x, y;
            char corb[32], corp[32];
            char ancora, conteudo[256];
            fscanf(arq_geo, "%d %lf %lf %31s %31s %c ",
                   &id, &x, &y, corb, corp, &ancora);
            fgets(conteudo, sizeof(conteudo), arq_geo);
            int len = strlen(conteudo);
            if (len > 0 && conteudo[len - 1] == '\n')
                conteudo[len - 1] = '\0';
            Forma *f = forma_cria_texto(id, x, y, corb, corp, ancora, conteudo);
            if (f)
                inserirArvore(formas, f);
        }
        else if (strcmp(cmd, "ts") == 0)
        {
            /* Estilo de texto: ts familia peso tamanho
             * Ignorado, apenas consumimos os parâmetros */
            char familia[32], peso[32], tamanho[32];
            fscanf(arq_geo, "%31s %31s %31s", familia, peso, tamanho);
        }
    }
}