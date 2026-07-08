#include <stdlib.h>
#include "fila.h"

/*
 * ============================================================
 * ESTRUTURA INTERNA DA FILA
 * ============================================================
 */

/**
 * Fila – implementação circular estática.
 *
 * Utiliza um array estatico de ponteiros genericos com tamanho FILA_MAX.
 * A fila é circular: quando 'fim' atinge o final do array, ele "dá a volta"
 * para o início se houver espaço disponível.
 *
 * - 'ini' : índice do elemento mais antigo (frente da fila)
 * - 'fim' : índice da próxima posição vazia disponível
 * - 'qtd' : quantidade atual de elementos
 */
struct Fila
{
    void *dados[FILA_MAX];
    int ini;
    int fim;
    int qtd;
};

/*
 * ============================================================
 * CONSTRUTOR E DESTRUIDOR
 * ============================================================
 */

/**
 * fila_cria – aloca e inicializa uma nova fila.
 *
 * A alocação requer apenas espaço para a estrutura, pois o array
 * interno já tem tamanho fixo. Todos os índices são zerados.
 */
Fila *fila_cria()
{
    Fila *f = (Fila *)malloc(sizeof(Fila));
    if (f == NULL)
        return NULL;
    f->ini = 0;
    f->fim = 0;
    f->qtd = 0;
    return f;
}

/**
 * fila_destroi – libera a memória da fila.
 *
 * Importante: esta função NÃO libera os dados armazenados.
 * O usuário deve liberar os dados antes de destruir a fila.
 */
void fila_destroi(Fila *f)
{
    if (f == NULL)
        return;
    free(f);
}

/*
 * ============================================================
 * OPERAÇÕES DE INSERÇÃO E REMOÇÃO
 * ============================================================
 */

/**
 * fila_insere – insere um elemento no final da fila (enqueue).
 *
 * Usa aritmética modular ((fim + 1) % FILA_MAX) para implementar
 * o comportamento circular. Se 'fim' chegar ao último índice do array
 * e houver espaço no início, ele "dá a volta" e volta para o índice 0.
 *
 * Isso garante operação O(1), sem necessidade de deslocar elementos.
 *
 * @return 1 em caso de sucesso, 0 se a fila estiver cheia.
 */
int fila_insere(Fila *f, void *dado)
{
    if (fila_cheia(f))
        return 0;

    f->dados[f->fim] = dado;
    f->fim = (f->fim + 1) % FILA_MAX;
    f->qtd++;

    return 1;
}

/**
 * fila_remove – remove e retorna o elemento mais antigo (dequeue).
 *
 * Similar à inserção, o índice 'ini' avança de forma circular.
 * Em vez de deslocar todos os elementos fisicamente para trás
 * (o que seria O(n)), apenas movemos o ponteiro inicial para a frente.
 *
 * @return Ponteiro para o dado removido, ou NULL se a fila estiver vazia.
 */
void *fila_remove(Fila *f)
{
    if (fila_vazia(f))
        return NULL;

    void *dado = f->dados[f->ini];
    f->ini = (f->ini + 1) % FILA_MAX;
    f->qtd--;

    return dado;
}

/**
 * fila_frente – consulta o elemento mais antigo sem remover.
 *
 * @return Ponteiro para o dado da frente, ou NULL se vazia.
 */
void *fila_frente(Fila *f)
{
    if (fila_vazia(f))
        return NULL;
    return f->dados[f->ini];
}

/*
 * ============================================================
 * ACESSO DIRETO A ELEMENTOS
 * ============================================================
 */

/**
 * fila_get – acessa um elemento pelo índice lógico.
 *
 * Mapeia um índice "lógico" (onde 0 é o primeiro da fila) para o
 * índice "físico" do array estático.
 *
 * Exemplo: se a fila começa no índice 98 e pedimos o item lógico 3,
 * a fórmula calcula (98 + 3) % 100 = índice físico 1.
 *
 * @param idx Índice lógico do elemento (0 = frente da fila).
 * @return Ponteiro para o dado, ou NULL se o índice for inválido.
 */
void *fila_get(Fila *f, int idx)
{
    if (idx < 0 || idx >= f->qtd)
        return NULL;
    return f->dados[(f->ini + idx) % FILA_MAX];
}

/*
 * ============================================================
 * CONSULTAS DE ESTADO
 * ============================================================
 */

int fila_tamanho(Fila *f) { return f->qtd; }
int fila_vazia(Fila *f) { return f->qtd == 0; }
int fila_cheia(Fila *f) { return f->qtd == FILA_MAX; }

/**
 * fila_limpa – esvazia a fila logicamente.
 *
 * O "reset" da fila é feito em tempo O(1) apenas reposicionando
 * os cursores, sem precisar limpar fisicamente os dados residuais
 * do array. Isso é seguro pois os índices sempre indicam a região
 * válida da fila, e os dados antigos serão sobrescritos quando
 * novas inserções ocorrerem.
 */
void fila_limpa(Fila *f)
{
    f->ini = 0;
    f->fim = 0;
    f->qtd = 0;
}