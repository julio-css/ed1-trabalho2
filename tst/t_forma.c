#include "unity.h"
#include "forma.h"

#include "stdio.h"

void setUp() {}
void tearDown() {}

void test_circulo() {
    Forma* f = forma_cria_circulo(1, 10.0, 20.0, 5.0, "red", "blue");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL_INT(1, forma_get_id(f));
    TEST_ASSERT_EQUAL_INT(FORMA_CIRCULO, forma_get_tipo(f));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 5.0, forma_get_raio(f));
    forma_destroi(f);
}

void test_retangulo() {
    Forma* f = forma_cria_retangulo(2, 0.0, 0.0, 100.0, 50.0, "black", "white");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL_INT(FORMA_RETANGULO, forma_get_tipo(f));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 100.0, forma_get_largura(f));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 50.0, forma_get_altura(f));
    forma_destroi(f);
}

void test_linha() {
    Forma* f = forma_cria_linha(3, 0.0, 0.0, 10.0, 10.0, "green");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL_INT(FORMA_LINHA, forma_get_tipo(f));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, forma_get_x2(f));
    forma_destroi(f);
}

void test_texto() {
    Forma* f = forma_cria_texto(4, 5.0, 5.0, "black", "white", 'i', "ola mundo");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL_INT(FORMA_TEXTO, forma_get_tipo(f));
    forma_destroi(f);
}

void test_set_cor() {
    Forma* f = forma_cria_circulo(1, 0.0, 0.0, 1.0, "red", "blue");
    forma_set_cor_borda(f, "green");
    TEST_ASSERT_EQUAL_STRING("green", forma_get_cor_borda(f));
    forma_destroi(f);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_circulo);
    RUN_TEST(test_retangulo);
    RUN_TEST(test_linha);
    RUN_TEST(test_texto);
    RUN_TEST(test_set_cor);
    return UNITY_END();
}
