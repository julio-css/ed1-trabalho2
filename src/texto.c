#include "texto.h"
#include "forma.h"

/*
 * ============================================================
 * CONSTRUTOR
 * ============================================================
 */

/**
 * texto_cria – delega a criação para forma_cria_texto.
 *
 * A criação efetiva e a alocação de memória são delegadas à função
 * genérica forma_cria_texto, centralizando a lógica de criação
 * no módulo forma.c.
 *
 * O módulo texto apenas fornece a interface específica para textos,
 * mas a implementação concreta fica no módulo forma.c, que gerencia
 * a união de dados específicos de cada tipo.
 *
 * @param id           Identificador único da forma (>= 1).
 * @param x            Coordenada X da âncora.
 * @param y            Coordenada Y da âncora.
 * @param cor_borda    Cor da borda (string válida, não NULL).
 * @param cor_preench  Cor do preenchimento (string válida, não NULL).
 * @param ancora_tipo  Posição da âncora: 'i' (início), 'm' (meio), 'f' (fim).
 * @param conteudo     Conteúdo do texto (string válida, não NULL).
 *
 * @return Ponteiro para a nova Forma, ou NULL em caso de falha.
 */
Forma *texto_cria(int id, double x, double y,
                  char *cor_borda, char *cor_preench,
                  char ancora_tipo, char *conteudo)
{
    return forma_cria_texto(id, x, y, cor_borda, cor_preench,
                            ancora_tipo, conteudo);
}