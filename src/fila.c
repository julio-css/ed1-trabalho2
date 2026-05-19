#include <stdlib.h>
#include "fila.h"

/*
 * estrutura interna da Fila.
 * utiliza um array estatico circular de ponteiros genericos.
 * 'ini' marca o indice do elemento mais antigo.
 * 'fim' marca a próxima posicao vazia disponivel.
 */
struct Fila {
    void* dados[FILA_MAX];
    int ini;
    int fim;
    int qtd;
};

/* a alocacao da Fila requer apenas espaco para a "carcaca" (a struct),
   visto que os ponteiros internos (o array) ja tem tamanho fixo.
 */
Fila* fila_cria() {
    Fila* f = (Fila*) malloc(sizeof(Fila));
    if (f == NULL) return NULL;
    f->ini = 0;
    f->fim = 0;
    f->qtd = 0;
    return f;
}

void fila_destroi(Fila* f) {
    if (f == NULL) return;
    free(f);
}

/*
 * insercao (enqueue)
 * Trabalhei usando aritmética modular ((fim + 1) % FILA_MAX). 
 * Se o 'fim' chegar ao ultimo indice do array e houver espaco no inicio,
   ele "da a volta" e volta para o indice 0. isso garante performance O(1).
 */
int fila_insere(Fila* f, void* dado) {
    if (fila_cheia(f)) return 0;
    
    f->dados[f->fim] = dado;
    f->fim = (f->fim + 1) % FILA_MAX;
    f->qtd++;
    
    return 1;
}

/*
 * remocao (dequeue)
 * fiz como na insercao, o indice 'ini' avanca de forma circular.
 * Em vez de deslocar todos os elementos fisicamente para tras (o que seria O(n)),
   apenas movemos o ponteiro inicial para a frente.
 */
void* fila_remove(Fila* f) {
    if (fila_vazia(f)) return NULL;
    
    void* dado = f->dados[f->ini];
    f->ini = (f->ini + 1) % FILA_MAX;
    f->qtd--;
    
    return dado;
}

void* fila_frente(Fila* f) {
    if (fila_vazia(f)) return NULL;
    return f->dados[f->ini];
}

/*
 * ACESSO DIRETO
 * Mapeia um indice "logico" (onde 0 eh o primeiro da fila) para o 
   indice "fisico" do array estatico.
 * exemplo: se a fila comeca no indice 98 e pedimos o item lógico 3, 
   a fórmula calcula (98 + 3) % 100 = indice fisico 1.
 */
void* fila_get(Fila* f, int idx) {
    if (idx < 0 || idx >= f->qtd) return NULL;
    return f->dados[(f->ini + idx) % FILA_MAX];
}

int fila_tamanho(Fila* f) { return f->qtd; }
int fila_vazia(Fila* f)   { return f->qtd == 0; }
int fila_cheia(Fila* f)   { return f->qtd == FILA_MAX; }

/* O "reset" da fila é feito em tempo O(1) apenas reposicionando 
   os cursores, sem precisar limpar fisicamente os dados residuais do array.
 */
void fila_limpa(Fila* f) {
    f->ini = 0;
    f->fim = 0;
    f->qtd = 0;
}