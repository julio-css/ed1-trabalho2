#include "unity.h"
#include "fila.h"

Fila* f;

void setUp() {
    f = fila_cria();
}

void tearDown() {
    fila_destroi(f);
}

void test_cria_vazia() {
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL_INT(1, fila_vazia(f));
    TEST_ASSERT_EQUAL_INT(0, fila_tamanho(f));
}

void test_insere_e_remove() {
    int x = 42;
    fila_insere(f, &x);
    TEST_ASSERT_EQUAL_INT(1, fila_tamanho(f));
    int* ret = (int*) fila_remove(f);
    TEST_ASSERT_EQUAL_INT(42, *ret);
    TEST_ASSERT_EQUAL_INT(1, fila_vazia(f));
}

void test_fifo() {
    int a = 1, b = 2, c = 3;
    fila_insere(f, &a);
    fila_insere(f, &b);
    fila_insere(f, &c);
    TEST_ASSERT_EQUAL_INT(1, *(int*)fila_remove(f));
    TEST_ASSERT_EQUAL_INT(2, *(int*)fila_remove(f));
    TEST_ASSERT_EQUAL_INT(3, *(int*)fila_remove(f));
}

void test_remove_vazia() {
    TEST_ASSERT_NULL(fila_remove(f));
}

void test_get() {
    int a = 10, b = 20;
    fila_insere(f, &a);
    fila_insere(f, &b);
    TEST_ASSERT_EQUAL_INT(10, *(int*)fila_get(f, 0));
    TEST_ASSERT_EQUAL_INT(20, *(int*)fila_get(f, 1));
    TEST_ASSERT_NULL(fila_get(f, 5));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_cria_vazia);
    RUN_TEST(test_insere_e_remove);
    RUN_TEST(test_fifo);
    RUN_TEST(test_remove_vazia);
    RUN_TEST(test_get);
    return UNITY_END();
}