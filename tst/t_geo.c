#include "unity.h"
#include "geo.h"
#include "forma.h"
#include "arvore.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"

void setUp(void)    {}
void tearDown(void) {}

/* =============================================================
 * geo_area
 * ============================================================= */

/*
 * test_area_circulo — pi * r^2
 */
static void test_area_circulo(void) {
    Forma* f = circulo_cria(1, 0, 0, 5.0, "black", "red");
    double esperado = 3.14159265358979 * 5.0 * 5.0;
    TEST_ASSERT_DOUBLE_WITHIN(0.001, esperado, geo_area(f));
    forma_destroi(f);
}

/*
 * test_area_retangulo — w * h
 */
static void test_area_retangulo(void) {
    Forma* f = retangulo_cria(2, 0, 0, 4.0, 3.0, "black", "blue");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 12.0, geo_area(f));
    forma_destroi(f);
}

/*
 * test_area_linha — 1.5 * comprimento
 * linha horizontal de comprimento 4: area = 1.5 * 4 = 6
 */
static void test_area_linha(void) {
    Forma* f = linha_cria(3, 0, 0, 4.0, 0.0, "black");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 6.0, geo_area(f));
    forma_destroi(f);
}

/*
 * test_area_texto — 10 * numero de caracteres
 * texto "ola" tem 3 chars: area = 30
 */
static void test_area_texto(void) {
    Forma* f = forma_cria_texto(4, 0, 0, "black", "white", 'i', "ola");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 30.0, geo_area(f));
    forma_destroi(f);
}

/* =============================================================
 * geo_largura e geo_altura
 * ============================================================= */

/*
 * test_largura_altura_circulo — ambas = 2 * r
 */
static void test_largura_altura_circulo(void) {
    Forma* f = circulo_cria(1, 0, 0, 3.0, "black", "red");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 6.0, geo_largura(f));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 6.0, geo_altura(f));
    forma_destroi(f);
}

/*
 * test_largura_altura_texto — largura = 1 * chars, altura = 10
 */
static void test_largura_altura_texto(void) {
    Forma* f = forma_cria_texto(4, 0, 0, "black", "white", 'i', "hello");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 5.0,  geo_largura(f));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, geo_altura(f));
    forma_destroi(f);
}

/*
 * test_largura_altura_linha — altura = 1.5 conforme spec
 */
static void test_largura_altura_linha(void) {
    Forma* f = linha_cria(3, 0, 0, 6.0, 0.0, "black");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 6.0, geo_largura(f));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.5, geo_altura(f));
    forma_destroi(f);
}

/* =============================================================
 * geo_comparar
 * ============================================================= */

/*
 * test_comparar_por_y — y menor vem antes
 */
static void test_comparar_por_y(void) {
    Forma* f1 = circulo_cria(1, 10, 5.0, 1.0, "black", "red");
    Forma* f2 = circulo_cria(2, 10, 9.0, 1.0, "black", "red");
    TEST_ASSERT_TRUE(geo_comparar(f1, f2) < 0);
    TEST_ASSERT_TRUE(geo_comparar(f2, f1) > 0);
    forma_destroi(f1);
    forma_destroi(f2);
}

/*
 * test_comparar_desempate_por_x — mesmo y, x menor vem antes
 */
static void test_comparar_desempate_por_x(void) {
    Forma* f1 = circulo_cria(1, 2.0, 5.0, 1.0, "black", "red");
    Forma* f2 = circulo_cria(2, 8.0, 5.0, 1.0, "black", "red");
    TEST_ASSERT_TRUE(geo_comparar(f1, f2) < 0);
    forma_destroi(f1);
    forma_destroi(f2);
}

/*
 * test_comparar_desempate_por_area — mesmo y e x, area menor vem antes
 */
static void test_comparar_desempate_por_area(void) {
    Forma* f1 = circulo_cria(1, 5.0, 5.0, 1.0, "black", "red"); /* area ~3.14 */
    Forma* f2 = circulo_cria(2, 5.0, 5.0, 2.0, "black", "red"); /* area ~12.56 */
    TEST_ASSERT_TRUE(geo_comparar(f1, f2) < 0);
    forma_destroi(f1);
    forma_destroi(f2);
}

/* =============================================================
 * geo_get_id
 * ============================================================= */

/*
 * test_get_id — retorna o id correto via void*
 */
static void test_get_id(void) {
    Forma* f = circulo_cria(42, 0, 0, 1.0, "black", "red");
    TEST_ASSERT_EQUAL_INT(42, geo_get_id((void*) f));
    forma_destroi(f);
}

/* =============================================================
 * geo_processa_arquivo — teste de integracao com arvore
 * ============================================================= */

/*
 * test_processa_geo_popula_arvore — le um .geo em memoria e
 * verifica que as formas foram inseridas na arvore
 */
static void test_processa_geo_popula_arvore(void) {
    /* monta um .geo minimo em arquivo temporario */
    FILE* tmp = tmpfile();
    fprintf(tmp, "c 1 10.0 20.0 5.0 black red\n");
    fprintf(tmp, "r 2 30.0 40.0 8.0 6.0 black blue\n");
    rewind(tmp);

    Arvore arv = criarArvore(geo_comparar);
    geo_processa_arquivo(tmp, arv);
    fclose(tmp);

    /* devem ter sido inseridos 2 elementos */
    TEST_ASSERT_EQUAL_INT(2, tamanhoArvore(arv));

    /* busca por id deve achar os dois */
    Forma* c = (Forma*) buscarPorIdArvore(arv, 1, geo_get_id);
    Forma* r = (Forma*) buscarPorIdArvore(arv, 2, geo_get_id);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_NOT_NULL(r);
    TEST_ASSERT_EQUAL_INT(FORMA_CIRCULO,    forma_get_tipo(c));
    TEST_ASSERT_EQUAL_INT(FORMA_RETANGULO,  forma_get_tipo(r));

    destruirArvore(arv, (FuncaoDestruir) forma_destroi);
}

/* =============================================================
 * runner
 * ============================================================= */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_area_circulo);
    RUN_TEST(test_area_retangulo);
    RUN_TEST(test_area_linha);
    RUN_TEST(test_area_texto);

    RUN_TEST(test_largura_altura_circulo);
    RUN_TEST(test_largura_altura_texto);
    RUN_TEST(test_largura_altura_linha);

    RUN_TEST(test_comparar_por_y);
    RUN_TEST(test_comparar_desempate_por_x);
    RUN_TEST(test_comparar_desempate_por_area);

    RUN_TEST(test_get_id);

    RUN_TEST(test_processa_geo_popula_arvore);

    return UNITY_END();
}