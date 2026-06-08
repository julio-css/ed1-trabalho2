#ifndef ARVORE_H
#define ARVORE_H

/* Ponteiro opaco que representa a Arvore */
typedef void* Arvore;

/*
 * Deve retornar:
 *   < 0  se elemento1 vem antes de elemento2
 *     0  se sao equivalentes na chave de ordenacao
 *   > 0  se elemento1 vem depois de elemento2
 */
typedef int (*FuncaoComparacao)(void* elemento1, void* elemento2);

/*
 * Chamada pela arvore ao destruir um no, se fornecida.
 * Libera apenas os dados internos do elemento — nao o no da arvore.
 */
typedef void (*FuncaoDestruir)(void* elemento);

/**
 * @brief Funcao de visitacao usada em travessias.
 *
 * @param elemento  Ponteiro para o dado armazenado no no visitado.
 * @param aux       Ponteiro auxiliar livre (lista destino, arquivo, etc).
 */
typedef void (*FuncaoVisita)(void* elemento, void* aux);

/**
 * @brief Funcao predicado usada como filtro em travessias.
 * 
 * Retorna 1 se o elemento deve ser visitado, 0 caso contrario.
 * Usada pelo sel, find e findrm para filtrar por regiao.
 *
 * @param elemento  Ponteiro para o dado a ser avaliado.
 * @param ctx       Contexto do filtro (ex: struct com x, y, w, h da regiao).
 */
typedef int (*FuncaoPredicado)(void* elemento, void* ctx);


/*FUNCOES PUBLICAS*/

/**
 * @brief Cria uma Arvore Binaria de Busca vazia.
 *
 * @pre  funcComp != NULL
 * @post Retorna uma arvore valida e vazia, ou NULL se falhar.
 * @param funcComp  Funcao que define o criterio de ordenacao da arvore.
 */
Arvore criarArvore(FuncaoComparacao funcComp);

/**
 * @brief Insere um elemento na arvore respeitando a funcao de comparacao.
 *
 * @pre  arvore != NULL, elemento != NULL
 * @post O elemento e posicionado corretamente na ABB.
 */
void inserirArvore(Arvore arvore, void* elemento);

/**
 * @brief Busca um elemento pela chave de ordenacao.
 *
 * Usa a funcao de comparacao da arvore para localizar o elemento.
 * Como a chave da arvore e (y, x, area) e nao o ID, use
 * buscarPorIdArvore para buscas por identificador numerico.
 *
 * @pre  arvore != NULL
 * @post A arvore permanece inalterada.
 *
 * @return Ponteiro para o elemento encontrado, ou NULL se nao existir.
 */
void* buscarArvore(Arvore arvore, void* chave);

/**
 * @brief Remove um elemento da arvore pela chave de ordenacao.
 *
 * A memoria do dado em si NAO e liberada aqui — o ponteiro e retornado
 * para que o chamador decida o que fazer.
 *
 * @pre  arvore != NULL
 * @post Se encontrado, a arvore tera um no a menos e sua estrutura
 *       sera reorganizada mantendo a propriedade da ABB.
 *
 * @return Ponteiro para o dado removido, ou NULL se nao encontrado.
 */
void* removerArvore(Arvore arvore, void* chave);

/**
 * @brief Percorre a arvore em ordem simetrica (In-Order).
 *
 * Visita todos os elementos na ordem crescente definida pela funcao
 * de comparacao. Util para gerar SVG e exportar formas em ordem.
 *
 * @pre  arvore != NULL, funcVisita != NULL
 * @post A arvore permanece inalterada.
 *
 * @param funcVisita  Funcao aplicada a cada elemento visitado.
 * @param aux         Ponteiro auxiliar repassado para funcVisita.
 */
void emOrdemArvore(Arvore arvore, FuncaoVisita funcVisita, void* aux);

/**
 * @brief Percorre em ordem aplicando funcVisita apenas nos elementos
 *        que passam no predicado.
 *
 * Diferente de emOrdemArvore, que visita todos os nos, esta funcao
 * chama funcVisita somente nos elementos para os quais predicado
 * retorna 1. Usada pelos comandos sel, find e findrm.
 *
 * @pre  arvore != NULL, predicado != NULL, funcVisita != NULL
 * @post A arvore permanece inalterada.
 *
 * @param predicado   Funcao que decide se o elemento sera visitado.
 * @param ctx         Contexto passado para o predicado (ex: regiao sel).
 * @param funcVisita  Funcao chamada para cada elemento que passa no filtro.
 * @param aux         Ponteiro auxiliar repassado para funcVisita.
 */
void emOrdemFiltroArvore(Arvore arvore,
                         FuncaoPredicado predicado, void* ctx,
                         FuncaoVisita    funcVisita, void* aux);

/**
 * @brief Busca um elemento pelo seu ID inteiro.
 *
 * Como o ID nao faz parte da chave de ordenacao (y, x, area), nao e
 * possivel usar a busca binaria da ABB. Esta funcao faz uma travessia
 * completa ate encontrar o elemento com o ID correspondente.
 *
 * A funcao de extracao de ID e fornecida pelo usuario para manter
 * o modulo arvore desacoplado do modulo Forma.
 *
 * @pre  arvore != NULL, funcGetId != NULL
 * @post A arvore permanece inalterada.
 *
 * @param id        Identificador inteiro do elemento buscado.
 * @param funcGetId Funcao que extrai o ID de um elemento generico.
 * @return          Ponteiro para o elemento encontrado, ou NULL.
 */
void* buscarPorIdArvore(Arvore arvore, int id, int (*funcGetId)(void*));

/**
 * @brief Retorna a quantidade de elementos armazenados na arvore.
 *
 * @pre  arvore != NULL
 * @post A arvore permanece inalterada.
 *
 * @return Numero de nos presentes na arvore.
 */
int tamanhoArvore(Arvore arvore);

/**
 * @brief Desaloca toda a memoria utilizada pela arvore.
 *
 * Remove todos os nos. Se funcDestruir for fornecida (nao NULL),
 * ela sera chamada para cada elemento antes de liberar o no.
 *
 * @pre  arvore != NULL
 * @post Toda a memoria da arvore e dos nos e liberada.
 *       O ponteiro arvore torna-se invalido apos esta chamada.
 *
 * @param funcDestruir  Funcao para liberar os dados de cada no,
 *                      ou NULL se os dados nao precisarem ser liberados.
 */
void destruirArvore(Arvore arvore, FuncaoDestruir funcDestruir);

#endif