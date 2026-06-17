#include "unity.h"
#include "sort.h"
#include <stdlib.h>
#include <string.h>

void setUp(void)    {}
void tearDown(void) {}

/* =============================================================
 * comparador simples de inteiros para os testes
 * ============================================================= */

static int cmp_int(void* a, void* b) {
    return (*(int*)a) - (*(int*)b);
}

/* =============================================================
 * helpers
 * ============================================================= */

/*
 * monta_vetor — aloca vetor de ponteiros para os valores do array
 * retorna o vetor; o chamador deve liberar com free()
 */
static void** monta_vetor(int* vals, int n) {
    void** v = (void**) malloc(n * sizeof(void*));
    for (int i = 0; i < n; i++) v[i] = &vals[i];
    return v;
}

/*
 * vetor_ordenado — retorna 1 se o vetor esta em ordem crescente
 */
static int vetor_ordenado(void** v, int n) {
    for (int i = 0; i < n-1; i++)
        if (cmp_int(v[i], v[i+1]) > 0) return 0;
    return 1;
}

/*
 * conta_snapshots — contexto simples para contar chamadas ao snapshot
 */
typedef struct { int count; } CtxContador;

static void snapshot_contador(void** v, int n, int i, int j, void* ctx) {
    (void)v; (void)n; (void)i; (void)j;
    CtxContador* c = (CtxContador*) ctx;
    c->count++;
}

/* =============================================================
 * Bubble Sort
 * ============================================================= */

static void test_bubble_ordena_sequencia_normal(void) {
    int vals[] = {5, 3, 1, 4, 2};
    void** v = monta_vetor(vals, 5);
    sort_bubble(v, 5, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

static void test_bubble_ja_ordenado(void) {
    int vals[] = {1, 2, 3, 4, 5};
    void** v = monta_vetor(vals, 5);
    sort_bubble(v, 5, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

static void test_bubble_um_elemento(void) {
    int vals[] = {42};
    void** v = monta_vetor(vals, 1);
    sort_bubble(v, 1, cmp_int, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(42, *(int*)v[0]);
    free(v);
}

static void test_bubble_chama_snapshot(void) {
    int vals[] = {3, 1, 2};
    void** v = monta_vetor(vals, 3);
    CtxContador ctx = {0};
    sort_bubble(v, 3, cmp_int, snapshot_contador, &ctx);
    /* bubble sort em {3,1,2} faz pelo menos 2 trocas */
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 3));
    free(v);
}

/* =============================================================
 * Selection Sort
 * ============================================================= */

static void test_selection_ordena_sequencia_normal(void) {
    int vals[] = {4, 2, 5, 1, 3};
    void** v = monta_vetor(vals, 5);
    sort_selection(v, 5, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

static void test_selection_chama_snapshot(void) {
    int vals[] = {4, 2, 1};
    void** v = monta_vetor(vals, 3);
    CtxContador ctx = {0};
    sort_selection(v, 3, cmp_int, snapshot_contador, &ctx);
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 3));
    free(v);
}

/* =============================================================
 * Insertion Sort
 * ============================================================= */

static void test_insertion_ordena_sequencia_normal(void) {
    int vals[] = {5, 2, 4, 1, 3};
    void** v = monta_vetor(vals, 5);
    sort_insertion(v, 5, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

static void test_insertion_chama_snapshot(void) {
    int vals[] = {3, 1, 2};
    void** v = monta_vetor(vals, 3);
    CtxContador ctx = {0};
    sort_insertion(v, 3, cmp_int, snapshot_contador, &ctx);
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 3));
    free(v);
}

/* =============================================================
 * Shell Sort
 * ============================================================= */

static void test_shell_ordena_sequencia_normal(void) {
    int vals[] = {9, 3, 7, 1, 8, 2, 6, 4, 5};
    void** v = monta_vetor(vals, 9);
    sort_shell(v, 9, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 9));
    free(v);
}

static void test_shell_chama_snapshot(void) {
    int vals[] = {5, 3, 1, 4, 2};
    void** v = monta_vetor(vals, 5);
    CtxContador ctx = {0};
    sort_shell(v, 5, cmp_int, snapshot_contador, &ctx);
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

/* =============================================================
 * Quick Sort
 * ============================================================= */

static void test_quick_ordena_sequencia_normal(void) {
    int vals[] = {3, 6, 8, 10, 1, 2, 1};
    void** v = monta_vetor(vals, 7);
    sort_quick(v, 7, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 7));
    free(v);
}

static void test_quick_chama_snapshot(void) {
    int vals[] = {4, 2, 5, 1, 3};
    void** v = monta_vetor(vals, 5);
    CtxContador ctx = {0};
    sort_quick(v, 5, cmp_int, snapshot_contador, &ctx);
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 5));
    free(v);
}

/* =============================================================
 * Merge Sort
 * ============================================================= */

static void test_merge_ordena_sequencia_normal(void) {
    int vals[] = {5, 2, 4, 6, 1, 3};
    void** v = monta_vetor(vals, 6);
    sort_merge(v, 6, cmp_int, NULL, NULL);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 6));
    free(v);
}

static void test_merge_chama_snapshot(void) {
    int vals[] = {3, 1, 2};
    void** v = monta_vetor(vals, 3);
    CtxContador ctx = {0};
    sort_merge(v, 3, cmp_int, snapshot_contador, &ctx);
    TEST_ASSERT_TRUE(ctx.count > 0);
    TEST_ASSERT_TRUE(vetor_ordenado(v, 3));
    free(v);
}

/* =============================================================
 * runner
 * ============================================================= */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_bubble_ordena_sequencia_normal);
    RUN_TEST(test_bubble_ja_ordenado);
    RUN_TEST(test_bubble_um_elemento);
    RUN_TEST(test_bubble_chama_snapshot);

    RUN_TEST(test_selection_ordena_sequencia_normal);
    RUN_TEST(test_selection_chama_snapshot);

    RUN_TEST(test_insertion_ordena_sequencia_normal);
    RUN_TEST(test_insertion_chama_snapshot);

    RUN_TEST(test_shell_ordena_sequencia_normal);
    RUN_TEST(test_shell_chama_snapshot);

    RUN_TEST(test_quick_ordena_sequencia_normal);
    RUN_TEST(test_quick_chama_snapshot);

    RUN_TEST(test_merge_ordena_sequencia_normal);
    RUN_TEST(test_merge_chama_snapshot);

    return UNITY_END();
}