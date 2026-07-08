#ifndef FORMA_H
#define FORMA_H

/**
 * @defgroup forma Forma
 * @brief Módulo base polimórfico para formas geométricas.
 *
 * 'Forma' é o tipo base genérico que representa qualquer figura geométrica.
 * A struct interna é "opaca" (definida somente em forma.c) para garantir
 * encapsulamento estrito. O acesso aos dados é feito EXCLUSIVAMENTE
 * pelas funções (getters/setters).
 *
 * @warning Nunca acesse os campos da struct Forma diretamente.
 *          Use sempre as funções fornecidas.
 *
 * @see circulo.h, retangulo.h, linha.h, texto.h, poligono.h
 * @{
 */

/** Tamanho máximo para strings de cor (incluindo terminador nulo). */
#define MAX_COR 32

/** Tamanho máximo para texto (incluindo terminador nulo). */
#define MAX_TEXTO 256

/* Estrutura opaca da forma (definida apenas no arquivo .c). */
typedef struct Forma Forma;

/**
 * @brief Tipo de forma, atua como uma "etiqueta" para o polimorfismo.
 *
 * Indica qual figura geométrica a Forma genérica está armazenando no momento,
 * determinando quais campos específicos (raio, largura, texto) são válidos.
 */
typedef enum
{
    FORMA_CIRCULO,    /**< Círculo (usa raio). */
    FORMA_RETANGULO,  /**< Retângulo (usa largura e altura). */
    FORMA_LINHA,      /**< Linha (usa x2, y2). */
    FORMA_TEXTO,      /**< Texto (usa texto e âncora). */
    FORMA_POLIGONO    /**< Polígono (usa arrays de pontos). */
} TipoForma;

/* ============================================================
 * CONSTRUTORES
 * ============================================================ */

/**
 * @brief Cria uma nova forma do tipo círculo.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X do centro.
 * @param y            Coordenada Y do centro.
 * @param r            Raio do círculo (> 0).
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, r > 0, cor_borda != NULL, cor_preench != NULL.
 */
Forma *forma_cria_circulo(int id, double x, double y, double r,
                          char *cor_borda, char *cor_preench);

/**
 * @brief Cria uma nova forma do tipo retângulo.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X do canto inferior esquerdo.
 * @param y            Coordenada Y do canto inferior esquerdo.
 * @param w            Largura do retângulo (> 0).
 * @param h            Altura do retângulo (> 0).
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, w > 0, h > 0, cor_borda != NULL, cor_preench != NULL.
 */
Forma *forma_cria_retangulo(int id, double x, double y, double w, double h,
                            char *cor_borda, char *cor_preench);

/**
 * @brief Cria uma nova forma do tipo linha.
 *
 * @param id   Identificador único da forma (>= 1).
 * @param x1   Coordenada X da primeira extremidade.
 * @param y1   Coordenada Y da primeira extremidade.
 * @param x2   Coordenada X da segunda extremidade.
 * @param y2   Coordenada Y da segunda extremidade.
 * @param cor  Cor da linha (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, cor != NULL.
 */
Forma *forma_cria_linha(int id, double x1, double y1, double x2, double y2,
                        char *cor);

/**
 * @brief Cria uma nova forma do tipo texto.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X da âncora.
 * @param y            Coordenada Y da âncora.
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 * @param ancora_tipo  Posição da âncora: 'i' (início), 'm' (meio), 'f' (fim).
 * @param texto        Conteúdo do texto (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, cor_borda != NULL, cor_preench != NULL, texto != NULL.
 * @pre ancora_tipo deve ser 'i', 'm' ou 'f'.
 */
Forma *forma_cria_texto(int id, double x, double y,
                        char *cor_borda, char *cor_preench,
                        char ancora_tipo, char *texto);

/**
 * @brief Cria uma nova forma do tipo polígono.
 *
 * @param id          Identificador único da forma (>= 1).
 * @param num_pontos  Número de vértices do polígono (>= 3).
 * @param pts_x       Array com as coordenadas X dos vértices.
 * @param pts_y       Array com as coordenadas Y dos vértices.
 * @param cor_borda   Cor da borda (string válida, não NULL).
 * @param cor_preench Cor do preenchimento (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 *
 * @pre id >= 1, num_pontos >= 3, pts_x != NULL, pts_y != NULL.
 * @pre cor_borda != NULL, cor_preench != NULL.
 * @note Os arrays pts_x e pts_y são copiados internamente.
 */
Forma *forma_cria_poligono(int id, int num_pontos,
                           double *pts_x, double *pts_y,
                           char *cor_borda, char *cor_preench);

/* ============================================================
 * DESTRUIDOR
 * ============================================================ */

/**
 * @brief Libera a memória alocada para uma forma.
 *
 * @param f Ponteiro para a forma a ser destruída.
 *
 * @pre f != NULL.
 * @post A memória da forma é liberada.
 */
void forma_destroi(Forma *f);

/* ============================================================
 * GETTERS (Leitura de Dados)
 * ============================================================ */

/**
 * @brief Obtém o identificador da forma.
 *
 * @param f Ponteiro para a forma.
 * @return ID da forma (int).
 * @pre f != NULL.
 */
int forma_get_id(Forma *f);

/**
 * @brief Obtém o tipo da forma.
 *
 * @param f Ponteiro para a forma.
 * @return Tipo da forma (TipoForma).
 * @pre f != NULL.
 */
TipoForma forma_get_tipo(Forma *f);

/** @name Coordenadas básicas */
/** @{ */
double forma_get_x(Forma *f);      /**< Obtém coordenada X. */
double forma_get_y(Forma *f);      /**< Obtém coordenada Y. */
double forma_get_x2(Forma *f);     /**< Obtém coordenada X2 (linha). */
double forma_get_y2(Forma *f);     /**< Obtém coordenada Y2 (linha). */
/** @} */

/** @name Dimensões específicas */
/** @{ */
double forma_get_raio(Forma *f);       /**< Obtém raio (círculo). */
double forma_get_largura(Forma *f);    /**< Obtém largura (retângulo). */
double forma_get_altura(Forma *f);     /**< Obtém altura (retângulo). */
/** @} */

/** @name Atributos visuais */
/** @{ */
char *forma_get_cor_borda(Forma *f);    /**< Obtém cor da borda. */
char *forma_get_cor_preench(Forma *f);  /**< Obtém cor do preenchimento. */
/** @} */

/** @name Atributos de texto */
/** @{ */
char *forma_get_texto(Forma *f);   /**< Obtém o conteúdo do texto. */
char forma_get_ancora(Forma *f);   /**< Obtém a posição da âncora: 'i', 'm' ou 'f'. */
/** @} */

/** @name Atributos de polígono */
/** @{ */
int forma_get_num_pontos(Forma *f);         /**< Obtém número de vértices. */
double forma_get_ponto_x(Forma *f, int i);  /**< Obtém X do i-ésimo vértice. */
double forma_get_ponto_y(Forma *f, int i);  /**< Obtém Y do i-ésimo vértice. */
/** @} */

/* ============================================================
 * SETTERS (Alteração de Dados)
 * ============================================================ */

/** @name Modificadores básicos */
/** @{ */
void forma_set_x(Forma *f, double x);      /**< Altera coordenada X. */
void forma_set_y(Forma *f, double y);      /**< Altera coordenada Y. */
void forma_set_x2(Forma *f, double x2);    /**< Altera coordenada X2 (linha). */
void forma_set_y2(Forma *f, double y2);    /**< Altera coordenada Y2 (linha). */
/** @} */

/** @name Modificadores visuais */
/** @{ */
void forma_set_cor_borda(Forma *f, char *cor);   /**< Altera cor da borda. */
void forma_set_cor_preench(Forma *f, char *cor); /**< Altera cor do preenchimento. */
/** @} */

/** @name Modificadores de polígono */
/** @{ */
void forma_set_ponto_x(Forma *f, int i, double x); /**< Altera X do i-ésimo vértice. */
void forma_set_ponto_y(Forma *f, int i, double y); /**< Altera Y do i-ésimo vértice. */
/** @} */

/* ============================================================
 * UTILITÁRIOS
 * ============================================================ */

/**
 * @brief Cria uma cópia profunda de uma Forma.
 *
 * O clone é completamente independente do original:
 * modificar um não afeta o outro.
 * Para polígonos, os arrays de pontos são copiados.
 *
 * @param f Ponteiro para a forma a ser clonada.
 *
 * @return Nova Forma idêntica, ou NULL em caso de falha.
 *
 * @pre f != NULL.
 * @post Retorna uma nova forma independente.
 */
Forma *forma_clona(Forma *f);

/** @} */ /* end of forma group */

#endif /* FORMA_H */