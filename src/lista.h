#ifndef LISTA_H
#define LISTA_H

/**
 * @defgroup lista Lista
 * @brief Lista encadeada simples (singly linked list).
 *
 * Implementação de uma lista encadeada genérica que armazena ponteiros para
 * dados (void*). A estrutura da lista é opaca (definida apenas no .c).
 *
 * @warning Os dados armazenados NÃO são liberados pela lista.
 *          É responsabilidade do usuário liberar a memória dos dados.
 *
 * @see fila.h
 * @{
 */

/* Estrutura opaca da lista (definida apenas no arquivo .c). */
typedef struct Lista Lista;

/* ============================================================
 * CONSTRUTOR E DESTRUIDOR
 * ============================================================ */

/**
 * @brief Cria uma lista vazia.
 *
 * @return Ponteiro para a nova lista, ou NULL em caso de falha.
 *
 * @post Retorna uma lista vazia pronta para uso.
 */
Lista* lista_criar();

/**
 * @brief Libera todos os nós e a estrutura da lista.
 *
 * @note Esta função NÃO libera a memória dos dados (void*) armazenados.
 *       O usuário deve liberar os dados antes de destruir a lista.
 *
 * @param l Ponteiro para a lista a ser destruída.
 *
 * @pre l != NULL.
 */
void lista_destruir(Lista* l);

/* ============================================================
 * OPERAÇÕES DE INSERÇÃO
 * ============================================================ */

/**
 * @brief Insere um dado no final da lista.
 *
 * @param l    Ponteiro para a lista.
 * @param dado Ponteiro para o dado a ser inserido.
 *
 * @return 1 em caso de sucesso, 0 em caso de falha.
 *
 * @pre l != NULL.
 * @post O elemento é adicionado ao final da lista.
 */
int lista_inserir_fim(Lista* l, void* dado);

/**
 * @brief Insere um dado no início da lista.
 *
 * @param l    Ponteiro para a lista.
 * @param dado Ponteiro para o dado a ser inserido.
 *
 * @return 1 em caso de sucesso, 0 em caso de falha.
 *
 * @pre l != NULL.
 * @post O elemento é adicionado ao início da lista.
 */
int lista_insere_inicio(Lista* l, void* dado);

/* ============================================================
 * OPERAÇÕES DE REMOÇÃO E CONSULTA
 * ============================================================ */

/**
 * @brief Remove o nó que contém exatamente o ponteiro fornecido.
 *
 * @param l    Ponteiro para a lista.
 * @param dado Ponteiro para o dado a ser removido.
 *
 * @return Ponteiro para o dado removido, ou NULL se não encontrado.
 *
 * @pre l != NULL.
 * @post O nó correspondente é removido.
 */
void* lista_remove(Lista* l, void* dado);

/**
 * @brief Acessa um dado pelo índice.
 *
 * @param l   Ponteiro para a lista.
 * @param idx Índice do elemento (0 = primeiro).
 *
 * @return Ponteiro para o dado, ou NULL se o índice for inválido.
 *
 * @pre l != NULL, idx >= 0 e idx < lista_tamanho(l).
 */
void* lista_get(Lista* l, int idx);

/* ============================================================
 * CONSULTAS DE ESTADO
 * ============================================================ */

/**
 * @brief Retorna a quantidade de elementos na lista.
 *
 * @param l Ponteiro para a lista.
 *
 * @return Número de elementos (int).
 *
 * @pre l != NULL.
 */
int lista_tamanho(Lista* l);

/**
 * @brief Verifica se a lista está vazia.
 *
 * @param l Ponteiro para a lista.
 *
 * @return 1 (verdadeiro) se vazia, 0 (falso) caso contrário.
 *
 * @pre l != NULL.
 */
int lista_vazia(Lista* l);

/** @} */ /* end of lista group */

#endif /* LISTA_H */