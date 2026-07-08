#ifndef FILA_H
#define FILA_H

/**
 * @defgroup fila Fila (Queue - FIFO)
 * @brief Implementação de uma fila circular estática baseada em ponteiros genéricos.
 *
 * A fila segue a política FIFO (First In, First Out). Utiliza um array estático
 * interno para evitar alocações dinâmicas em cada operação de inserção.
 * A estrutura interna é opaca (definida apenas no arquivo .c) para encapsulamento.
 * 
 * @warning O armazenamento é estático com capacidade máxima definida por FILA_MAX.
 *          Não é possível armazenar mais de FILA_MAX elementos simultaneamente.
 * @note Os dados armazenados são ponteiros void*, a fila não gerencia a memória
 *       dos dados; o usuário é responsável por liberá-los quando necessário.
 * @{
 */

/** Capacidade máxima da fila circular estática. */
#define FILA_MAX 100

/* Estrutura opaca da fila (detalhes internos no .c) */
typedef struct Fila Fila;

/**
 * @brief Inicializa a estrutura da fila.
 *
 * Aloca e inicializa uma nova fila vazia.
 *
 * @return Ponteiro para a nova Fila criada, ou NULL em caso de falha
 *         na alocação de memória.
 * @post Fila criada com tamanho 0.
 */
Fila* fila_cria();

/**
 * @brief Libera a memória alocada para a estrutura da fila.
 *
 * @warning Esta função NÃO libera a memória dos dados (void*) armazenados.
 *          É responsabilidade do usuário liberar os dados antes ou após
 *          destruir a fila.
 *
 * @param f Ponteiro para a fila.
 */
void fila_destroi(Fila* f);

/**
 * @brief Insere um novo elemento no fim da fila.
 *
 * @param f    Ponteiro para a fila (não NULL).
 * @param dado Ponteiro para o dado a ser inserido.
 *
 * @return 1 em caso de sucesso, 0 se a fila estiver cheia.
 * @pre f != NULL.
 * @post O dado é adicionado ao final da fila, se houver espaço.
 */
int fila_insere(Fila* f, void* dado);

/**
 * @brief Remove e devolve o elemento mais antigo (frente) da fila.
 *
 * @param f Ponteiro para a fila (não NULL).
 *
 * @return Ponteiro para o dado removido, ou NULL se a fila estiver vazia.
 * @pre f != NULL.
 * @post O elemento da frente é removido e seu ponteiro é retornado.
 */
void* fila_remove(Fila* f);

/**
 * @brief Consulta o elemento mais antigo (frente) da fila sem removê-lo.
 *
 * @param f Ponteiro para a fila (não NULL).
 *
 * @return Ponteiro para o dado da frente, ou NULL se a fila estiver vazia.
 * @pre f != NULL.
 */
void* fila_frente(Fila* f);

/**
 * @brief Acessa um elemento específico da fila por índice, sem removê-lo.
 *
 * O índice 0 corresponde ao elemento da frente (mais antigo).
 *
 * @param f   Ponteiro para a fila (não NULL).
 * @param idx Índice do elemento desejado (0 <= idx < tamanho atual).
 *
 * @return Ponteiro para o dado no índice especificado, ou NULL se o índice
 *         for inválido.
 * @pre f != NULL e 0 <= idx < fila_tamanho(f).
 */
void* fila_get(Fila* f, int idx);

/**
 * @brief Retorna a quantidade de elementos atualmente na fila.
 *
 * @param f Ponteiro para a fila (não NULL).
 *
 * @return Número de elementos armazenados.
 * @pre f != NULL.
 */
int fila_tamanho(Fila* f);

/**
 * @brief Verifica se a fila está vazia.
 *
 * @param f Ponteiro para a fila (não NULL).
 *
 * @return 1 (verdadeiro) se vazia, 0 (falso) caso contrário.
 * @pre f != NULL.
 */
int fila_vazia(Fila* f);

/**
 * @brief Verifica se a fila está cheia (atingiu a capacidade máxima).
 *
 * @param f Ponteiro para a fila (não NULL).
 *
 * @return 1 (verdadeiro) se cheia, 0 (falso) caso contrário.
 * @pre f != NULL.
 */
int fila_cheia(Fila* f);

/**
 * @brief Esvazia a fila logicamente (zera índices e contador).
 *
 * @warning Esta função NÃO libera a memória dos dados armazenados.
 *          Apenas reseta o estado da fila para vazia.
 *
 * @param f Ponteiro para a fila (não NULL).
 * @pre f != NULL.
 * @post A fila fica vazia, mas os dados continuam alocados externamente.
 */
void fila_limpa(Fila* f);

/** @} */ /* end of fila group */

#endif /* FILA_H */