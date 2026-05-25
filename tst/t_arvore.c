#include "unity.h"
#include "arvore.h"
#include <stdlib.h>

/**
 * @brief Função auxiliar para comparar dois inteiros nos testes.
 * Simula a função de comparação que depois faremos para as formas geométricas.
 */
int compararInt(void* a, void* b) {
    int v1 = *((int*)a);
    int v2 = *((int*)b);
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

/* O Unity exige a declaração de setUp e tearDown */
void setUp(void) {
    // Código executado antes de cada RUN_TEST
}

void tearDown(void) {
    // Código executado após cada RUN_TEST
}

/* --- CASOS DE TESTE --- */

void test_CriarArvore_DeveRetornarPonteiroNaoNulo(void) {
    Arvore arv = criarArvore(compararInt);
    TEST_ASSERT_NOT_NULL_MESSAGE(arv, "A arvore nao deveria ser NULL apos a criacao");
    destruirArvore(arv, NULL);
}

void test_InserirEBuscarArvore_DeveEncontrarElementosInseridos(void) {
    Arvore arv = criarArvore(compararInt);
    
    // Dados simulados
    int val1 = 10;
    int val2 = 5;
    int val3 = 15;
    int val4 = 20; // Valor que não será inserido

    // Testando inserção
    inserirArvore(arv, &val1);
    inserirArvore(arv, &val2);
    inserirArvore(arv, &val3);

    // Testando buscas com sucesso
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&val1, buscarArvore(arv, &val1), "Deveria encontrar a raiz (10)");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&val2, buscarArvore(arv, &val2), "Deveria encontrar o filho a esquerda (5)");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&val3, buscarArvore(arv, &val3), "Deveria encontrar o filho a direita (15)");

    // Testando busca de elemento inexistente
    TEST_ASSERT_NULL_MESSAGE(buscarArvore(arv, &val4), "Nao deveria encontrar um valor nao inserido");

    destruirArvore(arv, NULL);
}

void test_RemoverArvore_DeveRetornarElementoERemoverDaEstrutura(void) {
    Arvore arv = criarArvore(compararInt);
    
    int val1 = 50;
    int val2 = 30;
    int val3 = 70;

    inserirArvore(arv, &val1);
    inserirArvore(arv, &val2);
    inserirArvore(arv, &val3);

    // Removendo uma folha
    int* removido = (int*)removerArvore(arv, &val2);
    
    // Verifica se o ponteiro retornado é exatamente o ponteiro do dado
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&val2, removido, "O ponteiro retornado deve ser o do dado removido");
    
    // Verifica se ele realmente sumiu da árvore
    TEST_ASSERT_NULL_MESSAGE(buscarArvore(arv, &val2), "O valor 30 nao deveria mais ser encontrado na arvore");
    
    // Verifica se os outros continuam lá
    TEST_ASSERT_NOT_NULL_MESSAGE(buscarArvore(arv, &val1), "O valor 50 deveria continuar na arvore");
    TEST_ASSERT_NOT_NULL_MESSAGE(buscarArvore(arv, &val3), "O valor 70 deveria continuar na arvore");

    destruirArvore(arv, NULL);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_CriarArvore_DeveRetornarPonteiroNaoNulo);
    RUN_TEST(test_InserirEBuscarArvore_DeveEncontrarElementosInseridos);
    RUN_TEST(test_RemoverArvore_DeveRetornarElementoERemoverDaEstrutura);
    
    return UNITY_END();
}