#ifndef POLIGONO_H
#define POLIGONO_H

/**
 * @defgroup poligono Poligono
 * @brief Módulo para gerenciamento de polígonos (pontos de âncora).
 *
 * Um polígono é uma sequência de pontos (âncora de figuras) armazenados
 * em ordem de inserção numa fila circular estática. Os pontos são usados
 * para gerar linhas de borda e preenchimento pelo comando 'pol' do arquivo .qry.
 *
 * @see fila.h
 * @ingroup qry
 * @{
 */

/* Estrutura opaca do polígono (definida apenas no arquivo .c). */
typedef struct Poligono Poligono;

/* ============================================================
 * CONSTRUTOR E DESTRUIDOR
 * ============================================================ */

/**
 * @brief Cria um polígono vazio.
 *
 * @return Ponteiro para o novo polígono, ou NULL em caso de falha.
 *
 * @post Retorna um polígono vazio pronto para uso.
 */
Poligono* pol_cria();

/**
 * @brief Libera toda a memória alocada para o polígono.
 *
 * @param p Ponteiro para o polígono a ser destruído.
 *
 * @pre p != NULL.
 * @post A memória do polígono é liberada.
 */
void pol_destroi(Poligono* p);

/* ============================================================
 * OPERAÇÕES DE INSERÇÃO E REMOÇÃO
 * ============================================================ */

/**
 * @brief Insere uma coordenada no polígono.
 *
 * @param p Ponteiro para o polígono.
 * @param x Coordenada X do ponto.
 * @param y Coordenada Y do ponto.
 * @param id Identificador da forma associada.
 *
 * @return 1 em caso de sucesso, 0 se a fila estiver cheia.
 *
 * @pre p != NULL.
 * @post O ponto é adicionado ao final do polígono (FIFO).
 */
int pol_insere(Poligono* p, double x, double y, int id);

/**
 * @brief Remove a coordenada mais antiga do polígono (FIFO).
 *
 * @param p   Ponteiro para o polígono.
 * @param ox  Ponteiro para armazenar a coordenada X removida.
 * @param oy  Ponteiro para armazenar a coordenada Y removida.
 * @param oid Ponteiro para armazenar o ID removido.
 *
 * @return 1 em caso de sucesso, 0 se o polígono estiver vazio.
 *
 * @pre p != NULL.
 * @post O ponto mais antigo é removido e os dados são retornados.
 */
int pol_remove(Poligono* p, double* ox, double* oy, int* oid);

/**
 * @brief Remove todas as coordenadas do polígono.
 *
 * @param p Ponteiro para o polígono.
 *
 * @pre p != NULL.
 * @post O polígono fica vazio.
 */
void pol_limpa(Poligono* p);

/* ============================================================
 * CONSULTAS DE ESTADO
 * ============================================================ */

/**
 * @brief Retorna a quantidade de pontos no polígono.
 *
 * @param p Ponteiro para o polígono.
 *
 * @return Número de pontos (int).
 *
 * @pre p != NULL.
 */
int pol_tamanho(Poligono* p);

/**
 * @brief Verifica se o polígono está vazio.
 *
 * @param p Ponteiro para o polígono.
 *
 * @return 1 (verdadeiro) se vazio, 0 (falso) caso contrário.
 *
 * @pre p != NULL.
 */
int pol_vazio(Poligono* p);

/**
 * @brief Verifica se o polígono atingiu a capacidade máxima.
 *
 * @param p Ponteiro para o polígono.
 *
 * @return 1 (verdadeiro) se cheio, 0 (falso) caso contrário.
 *
 * @pre p != NULL.
 */
int pol_cheio(Poligono* p);

/* ============================================================
 * ACESSO A PONTOS
 * ============================================================ */

/**
 * @brief Acessa um ponto pelo índice sem removê-lo.
 *
 * @param p   Ponteiro para o polígono.
 * @param idx Índice do ponto (0 = mais antigo).
 * @param ox  Ponteiro para armazenar a coordenada X.
 * @param oy  Ponteiro para armazenar a coordenada Y.
 * @param oid Ponteiro para armazenar o ID.
 *
 * @return 1 se o índice é válido, 0 caso contrário.
 *
 * @pre p != NULL, idx >= 0.
 */
int pol_get(Poligono* p, int idx, double* ox, double* oy, int* oid);

/** @} */ /* end of poligono group */

#endif /* POLIGONO_H */