#include "unity.h"
#include "poligono.h"

void setUp(void) {}
void tearDown(void) {}

void test_pol_cria_destroi(void) {
    Poligono* p = pol_cria();
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_TRUE(pol_vazio(p));
    pol_destroi(p);
}

void test_pol_insere_remove(void) {
    Poligono* p = pol_cria();
    double x, y; int id;
    TEST_ASSERT_TRUE(pol_insere(p, 10.0, 20.0, 1));
    TEST_ASSERT_EQUAL_INT(1, pol_tamanho(p));
    TEST_ASSERT_TRUE(pol_remove(p, &x, &y, &id));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, x);
    TEST_ASSERT_EQUAL_DOUBLE(20.0, y);
    TEST_ASSERT_EQUAL_INT(1, id);
    TEST_ASSERT_TRUE(pol_vazio(p));
    pol_destroi(p);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pol_cria_destroi);
    RUN_TEST(test_pol_insere_remove);
    return UNITY_END();
}