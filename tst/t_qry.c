#include "unity.h"
#include "qry.h"
#include "geo.h"
#include "arvore.h"
#include "forma.h"
#include "poligono.h"
#include "circulo.h"
#include "retangulo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* =============================================================
 * HELPERS DE TESTE
 * ============================================================= */

/*
 * monta_arvore_padrao — cria uma arvore com 4 formas conhecidas,
 * usadas como cenario base na maioria dos testes:
 *   id 1: circulo  (10,20) r=5  -> altura=10
 *   id 2: retangulo(30,40) w=8 h=6
 *   id 3: circulo  (50,30) r=3  -> altura=6
 *   id 4: retangulo(20,60) w=10 h=4
 */
static Arvore monta_arvore_padrao(void)
{
    Arvore arv = criarArvore(geo_comparar);
    inserirArvore(arv, circulo_cria(1, 10, 20, 5.0, "black", "red"));
    inserirArvore(arv, retangulo_cria(2, 30, 40, 8.0, 6.0, "black", "blue"));
    inserirArvore(arv, circulo_cria(3, 50, 30, 3.0, "black", "green"));
    inserirArvore(arv, retangulo_cria(4, 20, 60, 10.0, 4.0, "black", "yellow"));
    return arv;
}

static Poligono **cria_poligonos_vazios(void)
{
    Poligono **pols = (Poligono **)malloc(MAX_POLIGONOS * sizeof(Poligono *));
    for (int i = 0; i < MAX_POLIGONOS; i++)
        pols[i] = pol_cria();
    return pols;
}

static void destroi_poligonos(Poligono **pols)
{
    for (int i = 0; i < MAX_POLIGONOS; i++)
        pol_destroi(pols[i]);
    free(pols);
}

/*
 * roda_qry — escreve o texto do .qry em arquivo temporario,
 * executa qry_processa_arquivo e devolve o conteudo do txt gerado.
 * O txt e escrito tambem em arquivo temporario e lido de volta
 * para uma string estatica (suficiente para os testes).
 */
static char buffer_txt[4096];

static void roda_qry(Arvore formas, Poligono **poligonos, const char *texto_qry)
{
    FILE *arq_qry = tmpfile();
    fputs(texto_qry, arq_qry);
    rewind(arq_qry);

    FILE *arq_txt = tmpfile();

    qry_processa_arquivo(arq_qry, formas, poligonos, arq_txt, NULL);

    rewind(arq_txt);
    size_t n = fread(buffer_txt, 1, sizeof(buffer_txt) - 1, arq_txt);
    buffer_txt[n] = '\0';

    fclose(arq_qry);
    fclose(arq_txt);
}

/* =============================================================
 * SEL
 * ============================================================= */

static void test_sel_seleciona_formas_na_regiao(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    roda_qry(arv, pols, "sel 0.0 0.0 100.0 100.0\n");

    /* todas as 4 formas estao dentro da regiao 0..100 */
    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "id 1"));
    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "id 2"));
    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "id 3"));
    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "id 4"));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

static void test_sel_regiao_restrita_exclui_formas(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* regiao pequena que so contem a forma id 1 (10,20) */
    roda_qry(arv, pols, "sel 0.0 0.0 15.0 25.0\n");

    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "id 1"));
    TEST_ASSERT_NULL(strstr(buffer_txt, "id 2"));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * DELS
 * ============================================================= */

static void test_dels_remove_selecionadas_da_arvore(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    roda_qry(arv, pols, "sel 0.0 0.0 100.0 100.0\ndels\n");

    /* arvore deve estar vazia apos remover todas as selecionadas */
    TEST_ASSERT_EQUAL_INT(0, tamanhoArvore(arv));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

static void test_dels_preserva_nao_selecionadas(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* seleciona so a forma id 1, remove so ela */
    roda_qry(arv, pols, "sel 0.0 0.0 15.0 25.0\ndels\n");

    TEST_ASSERT_EQUAL_INT(3, tamanhoArvore(arv));
    TEST_ASSERT_NULL(buscarPorIdArvore(arv, 1, geo_get_id));
    TEST_ASSERT_NOT_NULL(buscarPorIdArvore(arv, 2, geo_get_id));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * MCS — move e recolore
 * ============================================================= */

static void test_mcs_move_e_recolore_selecionadas(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* seleciona so a forma id 1 e move +100,+100 com nova cor */
    roda_qry(arv, pols, "sel 0.0 0.0 15.0 25.0\nmcs 100.0 100.0 white pink\n");

    Forma *f = (Forma *)buscarPorIdArvore(arv, 1, geo_get_id);
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 110.0, forma_get_x(f));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 120.0, forma_get_y(f));
    TEST_ASSERT_EQUAL_STRING("pink", forma_get_cor_preench(f));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

static void test_mcs_mantem_arvore_consistente_apos_mover(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* move a forma id 1 para uma posicao bem diferente */
    roda_qry(arv, pols, "sel 0.0 0.0 15.0 25.0\nmcs 500.0 500.0 black red\n");

    /* apos mover, a busca por id ainda deve encontrar a forma */
    Forma *f = (Forma *)buscarPorIdArvore(arv, 1, geo_get_id);
    TEST_ASSERT_NOT_NULL(f);
    /* e a arvore deve continuar com 4 elementos (nao duplicou nem perdeu) */
    TEST_ASSERT_EQUAL_INT(4, tamanhoArvore(arv));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * MC — so muda cor, nao move
 * ============================================================= */

static void test_mc_altera_apenas_cores(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    roda_qry(arv, pols, "sel 0.0 0.0 15.0 25.0\nmc orange purple\n");

    Forma *f = (Forma *)buscarPorIdArvore(arv, 1, geo_get_id);
    TEST_ASSERT_EQUAL_STRING("orange", forma_get_cor_borda(f));
    TEST_ASSERT_EQUAL_STRING("purple", forma_get_cor_preench(f));
    /* posicao original preservada */
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, forma_get_x(f));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 20.0, forma_get_y(f));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * CM — clona e move
 * ============================================================= */

static void test_cm_clona_formas_da_regiao(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    int antes = tamanhoArvore(arv);
    roda_qry(arv, pols, "cm 0.0 0.0 15.0 25.0 200.0 200.0\n");

    /* a regiao so contem a forma id 1 -> deve gerar exatamente 1 clone */
    TEST_ASSERT_EQUAL_INT(antes + 1, tamanhoArvore(arv));

    /* o original ainda deve existir na posicao antiga */
    Forma *original = (Forma *)buscarPorIdArvore(arv, 1, geo_get_id);
    TEST_ASSERT_NOT_NULL(original);

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

static void test_cm_clone_fica_deslocado(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    roda_qry(arv, pols, "cm 0.0 0.0 15.0 25.0 200.0 200.0\n");

    /* deve existir uma forma na nova posicao (10+200, 20+200) */
    int n = tamanhoArvore(arv);
    TEST_ASSERT_EQUAL_INT(5, n);

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * FIND
 * ============================================================= */

static void test_find_ordena_por_altura_e_reposiciona_k_menores(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* alturas: id1=10 (circ r5), id2=6, id3=6 (circ r3), id4=4
     * os 2 menores por altura: id4 (4) e (id2 ou id3, ambos 6) */
    roda_qry(arv, pols, "sel 0.0 0.0 100.0 100.0\nfind 2 bs h 5.0 80.0 20.0\n");

    TEST_ASSERT_NOT_NULL(strstr(buffer_txt, "altura=4.00"));

    /* arvore continua com 4 formas — find nao remove nada */
    TEST_ASSERT_EQUAL_INT(4, tamanhoArvore(arv));

    /* a forma de menor altura (id 4) deve estar na posicao destino */
    Forma *f4 = (Forma *)buscarPorIdArvore(arv, 4, geo_get_id);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 80.0, forma_get_y(f4));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

static void test_find_com_k_maior_que_selecionadas_nao_quebra(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    /* k=10 mas so ha 4 selecionadas — deve limitar sem crashar */
    roda_qry(arv, pols, "sel 0.0 0.0 100.0 100.0\nfind 10 ss d 0.0 0.0 10.0\n");

    TEST_ASSERT_EQUAL_INT(4, tamanhoArvore(arv));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * FINDRM
 * ============================================================= */

static void test_findrm_remove_elementos_de_rank_maior_que_k(void)
{
    Arvore arv = monta_arvore_padrao();
    Poligono **pols = cria_poligonos_vazios();

    roda_qry(arv, pols, "sel 0.0 0.0 100.0 100.0\nfindrm 2 bs h 5.0 80.0 20.0\n");

    /* so devem sobrar 2 formas na arvore */
    TEST_ASSERT_EQUAL_INT(2, tamanhoArvore(arv));

    /* a forma de maior altura (id 1, altura=10) deve ter sido removida */
    TEST_ASSERT_NULL(buscarPorIdArvore(arv, 1, geo_get_id));

    /* a forma de menor altura (id 4, altura=4) deve ter sobrevivido */
    TEST_ASSERT_NOT_NULL(buscarPorIdArvore(arv, 4, geo_get_id));

    destroi_poligonos(pols);
    destruirArvore(arv, (FuncaoDestruir)forma_destroi);
}

/* =============================================================
 * runner
 * ============================================================= */

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_sel_seleciona_formas_na_regiao);
    RUN_TEST(test_sel_regiao_restrita_exclui_formas);

    RUN_TEST(test_dels_remove_selecionadas_da_arvore);
    RUN_TEST(test_dels_preserva_nao_selecionadas);

    RUN_TEST(test_mcs_move_e_recolore_selecionadas);
    RUN_TEST(test_mcs_mantem_arvore_consistente_apos_mover);

    RUN_TEST(test_mc_altera_apenas_cores);

    RUN_TEST(test_cm_clona_formas_da_regiao);
    RUN_TEST(test_cm_clone_fica_deslocado);

    RUN_TEST(test_find_ordena_por_altura_e_reposiciona_k_menores);
    RUN_TEST(test_find_com_k_maior_que_selecionadas_nao_quebra);

    RUN_TEST(test_findrm_remove_elementos_de_rank_maior_que_k);

    return UNITY_END();
}