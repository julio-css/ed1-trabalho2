#ifndef ARVORE_H
#define ARVORE_H

/**
 * @file arvore.h
 * @brief Módulo de uma Árvore Binária de Busca (ABB) Genérica.
 * * Este módulo provê uma implementação de Árvore Binária de Busca capaz de armazenar qualquer tipo de dado através de ponteiros opacos (void*).
 * A ordenação dos elementos é definida por uma função de comparação fornecida pelo usuário no momento da criação da árvore.
 */

/* Ponteiro opaco que representa a Árvore Binária de Busca */
typedef void* Arvore;

/**
 * @brief Ponteiro para função de comparação de dois elementos.
 * * A função deve comparar dois elementos (definidos pelo usuário) e retornar:
 * - Um valor negativo se o primeiro elemento for menor que o segundo.
 * - Zero se o primeiro elemento for igual ao segundo.
 * - Um valor positivo se o primeiro elemento for maior que o segundo.
 * * @param elemento1 Ponteiro para o primeiro elemento.
 * @param elemento2 Ponteiro para o segundo elemento.
 * @return int Resultado da comparação (< 0, 0 ou > 0).
 */
typedef int (*FuncaoComparacao)(void* elemento1, void* elemento2);

/**
 * @brief Ponteiro para função de liberação de memória de um elemento.
 * * Função utilizada para desalocar a memória interna do dado armazenado na árvore,
 * caso necessário, durante a destruição da árvore ou remoção do nó.
 * * @param elemento Ponteiro para o elemento a ser desalocado.
 */
typedef void (*FuncaoDestruir)(void* elemento);

/**
 * @brief Ponteiro para função de visitação de elementos em travessias.
 * * @param elemento Ponteiro para o elemento armazenado no nó visitado.
 * @param aux Ponteiro auxiliar fornecido pelo usuário (pode ser uma lista, 
 * vetor, arquivo, etc.).
 */
typedef void (*FuncaoVisita)(void* elemento, void* aux);


/**
 * @brief Cria uma Árvore Binária de Busca vazia.
 * * @pre Nenhuma.
 * @post Uma instância de Arvore válida e vazia é criada em memória.
 * * @param funcComp Ponteiro para a função que define o critério de ordenação da árvore.
 * Não pode ser NULL.
 * @return Arvore Retorna o ponteiro para a árvore criada, ou NULL em caso de falha.
 */
Arvore criarArvore(FuncaoComparacao funcComp);

/**
 * @brief Insere um novo elemento na árvore de acordo com a função de comparação.
 * * @pre 'arvore' deve ter sido criada por criarArvore e não ser NULL. 'elemento' não pode ser NULL.
 * @post O elemento é posicionado corretamente na árvore respeitando a propriedade da ABB.
 * * @param arvore Ponteiro para a árvore.
 * @param elemento Ponteiro para o dado que se deseja armazenar.
 */
void inserirArvore(Arvore arvore, void* elemento);

/**
 * @brief Busca por um elemento na árvore que coincida com a chave informada.
 * * @pre 'arvore' não pode ser NULL.
 * @post A árvore permanece inalterada.
 * * @param arvore Ponteiro para a árvore.
 * @param chave Ponteiro para um elemento que contém os dados da busca (usado pela função de comparação).
 * @return void* Retorna o ponteiro para o elemento encontrado, ou NULL caso não exista.
 */
void* buscarArvore(Arvore arvore, void* chave);

/**
 * @brief Remove um elemento específico da árvore.
 * * Se o elemento for encontrado, ele é removido da estrutura da árvore, mantendo as 
 * propriedades da ABB intactas. A memória do dado em si NÃO é liberada por esta função; 
 * o ponteiro do dado removido é retornado para que o usuário decida o que fazer.
 * * @pre 'arvore' não pode ser NULL.
 * @post Se encontrado, a árvore terá um nó a menos e sua estrutura será reorganizada.
 * * @param arvore Ponteiro para a árvore.
 * @param chave Ponteiro para o elemento que se deseja remover (usado para busca).
 * @return void* Retorna o ponteiro para o dado que foi removido da árvore, ou NULL se não encontrado.
 */
void* removerArvore(Arvore arvore, void* chave);

/**
 * @brief Percorre a árvore em ordem simétrica (In-Order), aplicando uma função a cada elemento.
 * * Como a árvore está ordenada pelo critério default, a travessia Em-Ordem visitará 
 * os elementos exatamente na ordem crescente estabelecida pela função de comparação.
 * Útil para exportar as figuras ordenadas ou filtrá-las.
 * * @pre 'arvore' não pode ser NULL. 'funcVisita' não pode ser NULL.
 * @post A árvore permanece inalterada. Os elementos são visitados na ordem correta.
 * * @param arvore Ponteiro para a árvore.
 * @param funcVisita Função que será aplicada a cada elemento visitado.
 * @param aux Ponteiro auxiliar passado para a função de visita (pode ser usado para acumular dados).
 */
void emOrdemArvore(Arvore arvore, FuncaoVisita funcVisita, void* aux);

/**
 * @brief Desaloca toda a memória utilizada pela árvore.
 * * Remove todos os nós da árvore. Se 'funcDestruir' for fornecida (não NULL), 
 * ela será chamada para cada elemento armazenado para liberar a memória do dado customizado.
 * * @pre 'arvore' não pode ser NULL.
 * @post A memória de todos os nós (e opcionalmente dos dados) é liberada. O ponteiro 'arvore' torna-se inválido.
 * * @param arvore Ponteiro para a árvore que será destruída.
 * @param funcDestruir Função para liberar os dados de cada nó, ou NULL se os dados não precisarem ser liberados aqui.
 */
void destruirArvore(Arvore arvore, FuncaoDestruir funcDestruir);

#endif 