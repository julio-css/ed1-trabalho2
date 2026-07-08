#include <stdlib.h>
#include <string.h>
#include "forma.h"

/*
 * ============================================================
 * ESTRUTURAS DE DADOS ESPECÍFICAS POR TIPO
 * ============================================================
 */

/**
 * DadosCirculo – armazena o raio do círculo.
 * Único campo específico para círculos.
 */
typedef struct
{
    double raio;
} DadosCirculo;

/**
 * DadosRetangulo – armazena largura e altura.
 */
typedef struct
{
    double w, h;
} DadosRetangulo;

/**
 * DadosLinha – armazena as coordenadas da segunda extremidade.
 * A primeira extremidade fica armazenada em (x, y) da Forma.
 */
typedef struct
{
    double x2, y2;
} DadosLinha;

/**
 * DadosTexto – armazena a âncora e o conteúdo do texto.
 */
typedef struct
{
    char ancora_tipo; /* 'i' (início), 'm' (meio) ou 'f' (fim) */
    char texto[MAX_TEXTO];
} DadosTexto;

/**
 * DadosPoligono – armazena os vértices do polígono.
 *
 * Usa arrays dinâmicos para permitir polígonos com qualquer número
 * de vértices (desde que >= 3 para gerar uma forma válida).
 * Os arrays são alocados quando o polígono é criado e liberados
 * quando a forma é destruída.
 */
typedef struct
{
    int num_pontos;
    double *pts_x; /* Array dinâmico para os eixos X */
    double *pts_y; /* Array dinâmico para os eixos Y */
} DadosPoligono;

/*
 * ============================================================
 * ESTRUTURA PRINCIPAL DA FORMA
 * ============================================================
 */

/**
 * Forma – tipo base polimórfico para todas as formas geométricas.
 *
 * A estrutura é opaca (definida no .c) para garantir encapsulamento.
 * O acesso aos campos é feito exclusivamente por getters/setters.
 *
 * A union 'dados' contém os dados específicos de cada tipo,
 * economizando memória ao compartilhar o espaço entre os diferentes
 * tipos de forma (apenas um tipo é válido por vez, indicado por 'tipo').
 */
struct Forma
{
    int id;                    /* Identificador único */
    double x, y;               /* Coordenadas da âncora */
    char cor_borda[MAX_COR];   /* Cor da borda */
    char cor_preench[MAX_COR]; /* Cor do preenchimento */
    TipoForma tipo;            /* Tipo da forma (determina qual campo da union é válido) */
    union
    {
        DadosCirculo circ;  /* Dados específicos de círculo */
        DadosRetangulo ret; /* Dados específicos de retângulo */
        DadosLinha lin;     /* Dados específicos de linha */
        DadosTexto txt;     /* Dados específicos de texto */
        DadosPoligono pol;  /* Dados específicos de polígono */
    } dados;
};

/*
 * ============================================================
 * FUNÇÕES AUXILIARES (internas ao módulo)
 * ============================================================
 */

/**
 * aloca_forma – aloca e inicializa a estrutura base da Forma.
 *
 * Esta função é usada por todos os construtores para evitar
 * duplicação de código. Ela aloca a memória, preenche os campos
 * comuns (id, coordenadas, cores) e define o tipo da forma.
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 */
static Forma *aloca_forma(int id, double x, double y,
                          char *cor_borda, char *cor_preench,
                          TipoForma tipo)
{
    Forma *f = (Forma *)malloc(sizeof(Forma));
    if (f == NULL)
        return NULL;

    f->id = id;
    f->x = x;
    f->y = y;
    f->tipo = tipo;

    /* Copia as cores com segurança (garante terminador nulo) */
    strncpy(f->cor_borda, cor_borda, MAX_COR - 1);
    strncpy(f->cor_preench, cor_preench, MAX_COR - 1);
    f->cor_borda[MAX_COR - 1] = '\0';
    f->cor_preench[MAX_COR - 1] = '\0';

    return f;
}

/*
 * ============================================================
 * CONSTRUTORES
 * ============================================================
 */

/**
 * forma_cria_circulo – cria uma nova forma do tipo círculo.
 */
Forma *forma_cria_circulo(int id, double x, double y, double r,
                          char *cor_borda, char *cor_preench)
{
    Forma *f = aloca_forma(id, x, y, cor_borda, cor_preench, FORMA_CIRCULO);
    if (f != NULL)
        f->dados.circ.raio = r;
    return f;
}

/**
 * forma_cria_retangulo – cria uma nova forma do tipo retângulo.
 */
Forma *forma_cria_retangulo(int id, double x, double y,
                            double w, double h,
                            char *cor_borda, char *cor_preench)
{
    Forma *f = aloca_forma(id, x, y, cor_borda, cor_preench, FORMA_RETANGULO);
    if (f != NULL)
    {
        f->dados.ret.w = w;
        f->dados.ret.h = h;
    }
    return f;
}

/**
 * forma_cria_linha – cria uma nova forma do tipo linha.
 *
 * A primeira extremidade é armazenada em (x, y) e a segunda
 * em (x2, y2) dentro da union.
 */
Forma *forma_cria_linha(int id, double x1, double y1,
                        double x2, double y2, char *cor)
{
    Forma *f = aloca_forma(id, x1, y1, cor, cor, FORMA_LINHA);
    if (f != NULL)
    {
        f->dados.lin.x2 = x2;
        f->dados.lin.y2 = y2;
    }
    return f;
}

/**
 * forma_cria_texto – cria uma nova forma do tipo texto.
 */
Forma *forma_cria_texto(int id, double x, double y,
                        char *cor_borda, char *cor_preench,
                        char ancora_tipo, char *texto)
{
    Forma *f = aloca_forma(id, x, y, cor_borda, cor_preench, FORMA_TEXTO);
    if (f != NULL)
    {
        f->dados.txt.ancora_tipo = ancora_tipo;
        strncpy(f->dados.txt.texto, texto, MAX_TEXTO - 1);
        f->dados.txt.texto[MAX_TEXTO - 1] = '\0';
    }
    return f;
}

/**
 * forma_cria_poligono – cria uma nova forma do tipo polígono.
 *
 * Copia os arrays de pontos para dentro da estrutura, alocando
 * memória dinamicamente. A âncora principal fica em (0, 0) pois
 * o polígono é definido pelos seus vértices.
 *
 * @param num_pontos Número de vértices (>= 3 para uma forma válida).
 */
Forma *forma_cria_poligono(int id, int num_pontos,
                           double *pts_x, double *pts_y,
                           char *cor_borda, char *cor_preench)
{
    /* Passamos (0,0) como âncora principal, pois a área do polígono
     * é definida pelos seus vértices, não por uma âncora única. */
    Forma *f = aloca_forma(id, 0, 0, cor_borda, cor_preench, FORMA_POLIGONO);
    if (f == NULL)
        return NULL;

    f->dados.pol.num_pontos = num_pontos;
    f->dados.pol.pts_x = (double *)malloc(num_pontos * sizeof(double));
    f->dados.pol.pts_y = (double *)malloc(num_pontos * sizeof(double));

    /* Copia os pontos para os arrays internos */
    for (int i = 0; i < num_pontos; i++)
    {
        f->dados.pol.pts_x[i] = pts_x[i];
        f->dados.pol.pts_y[i] = pts_y[i];
    }

    return f;
}

/*
 * ============================================================
 * DESTRUIDOR
 * ============================================================
 */

/**
 * forma_destroi – libera a memória de uma forma.
 *
 * Para polígonos, também libera os arrays dinâmicos de pontos.
 * Para outros tipos, apenas libera a estrutura principal.
 */
void forma_destroi(Forma *f)
{
    if (f == NULL)
        return;

    /* Polígonos têm arrays dinâmicos que precisam ser liberados */
    if (f->tipo == FORMA_POLIGONO)
    {
        free(f->dados.pol.pts_x);
        free(f->dados.pol.pts_y);
    }
    free(f);
}

/*
 * ============================================================
 * GETTERS (Consulta de dados)
 * ============================================================
 */

/* Getters básicos – acesso direto a campos da struct */
int forma_get_id(Forma *f) { return f->id; }
TipoForma forma_get_tipo(Forma *f) { return f->tipo; }
double forma_get_x(Forma *f) { return f->x; }
double forma_get_y(Forma *f) { return f->y; }
char *forma_get_cor_borda(Forma *f) { return f->cor_borda; }
char *forma_get_cor_preench(Forma *f) { return f->cor_preench; }

/* Getters específicos – acessam os campos da union */
double forma_get_x2(Forma *f) { return f->dados.lin.x2; }
double forma_get_y2(Forma *f) { return f->dados.lin.y2; }
double forma_get_raio(Forma *f) { return f->dados.circ.raio; }
double forma_get_largura(Forma *f) { return f->dados.ret.w; }
double forma_get_altura(Forma *f) { return f->dados.ret.h; }
char *forma_get_texto(Forma *f) { return f->dados.txt.texto; }
char forma_get_ancora(Forma *f) { return f->dados.txt.ancora_tipo; }

/* Getters específicos para polígono */
int forma_get_num_pontos(Forma *f) { return f->dados.pol.num_pontos; }
double forma_get_ponto_x(Forma *f, int i) { return f->dados.pol.pts_x[i]; }
double forma_get_ponto_y(Forma *f, int i) { return f->dados.pol.pts_y[i]; }

/*
 * ============================================================
 * SETTERS (Modificação de dados)
 * ============================================================
 */

/* Setters básicos – modificam campos da struct */
void forma_set_x(Forma *f, double x) { f->x = x; }
void forma_set_y(Forma *f, double y) { f->y = y; }

/* Setters específicos para linha */
void forma_set_x2(Forma *f, double x2) { f->dados.lin.x2 = x2; }
void forma_set_y2(Forma *f, double y2) { f->dados.lin.y2 = y2; }

/* Setters específicos para polígono */
void forma_set_ponto_x(Forma *f, int i, double x) { f->dados.pol.pts_x[i] = x; }
void forma_set_ponto_y(Forma *f, int i, double y) { f->dados.pol.pts_y[i] = y; }

/**
 * forma_set_cor_borda – altera a cor da borda.
 */
void forma_set_cor_borda(Forma *f, char *cor)
{
    strncpy(f->cor_borda, cor, MAX_COR - 1);
    f->cor_borda[MAX_COR - 1] = '\0';
}

/**
 * forma_set_cor_preench – altera a cor do preenchimento.
 */
void forma_set_cor_preench(Forma *f, char *cor)
{
    strncpy(f->cor_preench, cor, MAX_COR - 1);
    f->cor_preench[MAX_COR - 1] = '\0';
}

/*
 * ============================================================
 * CLONAGEM
 * ============================================================
 */

/**
 * forma_clona – cria uma cópia profunda de uma Forma.
 *
 * Usa os construtores existentes para garantir que toda alocação
 * interna (especialmente para polígonos) seja feita corretamente.
 * O clone é completamente independente do original.
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 */
Forma *forma_clona(Forma *f)
{
    if (f == NULL)
        return NULL;

    switch (f->tipo)
    {
    case FORMA_CIRCULO:
        return forma_cria_circulo(
            f->id, f->x, f->y,
            f->dados.circ.raio,
            f->cor_borda, f->cor_preench);

    case FORMA_RETANGULO:
        return forma_cria_retangulo(
            f->id, f->x, f->y,
            f->dados.ret.w, f->dados.ret.h,
            f->cor_borda, f->cor_preench);

    case FORMA_LINHA:
        return forma_cria_linha(
            f->id, f->x, f->y,
            f->dados.lin.x2, f->dados.lin.y2,
            f->cor_borda);

    case FORMA_TEXTO:
        return forma_cria_texto(
            f->id, f->x, f->y,
            f->cor_borda, f->cor_preench,
            f->dados.txt.ancora_tipo,
            f->dados.txt.texto);

    case FORMA_POLIGONO:
        return forma_cria_poligono(
            f->id,
            f->dados.pol.num_pontos,
            f->dados.pol.pts_x,
            f->dados.pol.pts_y,
            f->cor_borda, f->cor_preench);

    default:
        return NULL;
    }
}