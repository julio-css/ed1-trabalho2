#include "unity.h"
#include "lista.h"

Lista* l;

void setUp() {
    l = lista_criar();
}

void tearDown() {
    lista_destruir(l);
}

void test_cria_vazia() {
    TEST_ASSERT_NOT_NULL(l);
    TEST_ASSERT_EQUAL_INT(1, lista_vazia(l));
    TEST_ASSERT_EQUAL_INT(0, lista_tamanho(l));
}

void test_inserir_fim() {
    int a = 1, b = 2;
    lista_inserir_fim(l, &a);
    lista_inserir_fim(l, &b);
    TEST_ASSERT_EQUAL_INT(2, lista_tamanho(l));
    TEST_ASSERT_EQUAL_INT(1, *(int*)lista_get(l, 0));
    TEST_ASSERT_EQUAL_INT(2, *(int*)lista_get(l, 1));
}

void test_insere_inicio() {
    int a = 1, b = 2;
    lista_inserir_fim(l, &a);
    lista_insere_inicio(l, &b);
    TEST_ASSERT_EQUAL_INT(2, *(int*)lista_get(l, 0));
    TEST_ASSERT_EQUAL_INT(1, *(int*)lista_get(l, 1));
}

void test_remove() {
    int a = 1, b = 2;
    lista_inserir_fim(l, &a);
    lista_inserir_fim(l, &b);
    int* ret = (int*) lista_remove(l, &a);
    TEST_ASSERT_EQUAL_INT(1, *ret);
    TEST_ASSERT_EQUAL_INT(1, lista_tamanho(l));
}

void test_get_invalido() {
    TEST_ASSERT_NULL(lista_get(l, 0));
    TEST_ASSERT_NULL(lista_get(l, -1));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_cria_vazia);
    RUN_TEST(test_inserir_fim);
    RUN_TEST(test_insere_inicio);
    RUN_TEST(test_remove);
    RUN_TEST(test_get_invalido);
    return UNITY_END();
}
