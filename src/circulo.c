#include <math.h>
#include "circulo.h"
#include "forma.h"

/* a criacao efetiva e a alocacao de memoria sao delegadas a função 
   genérica forma_cria_circulo. Isso garante que o preenchimento da 
   struct principal (id, coordenadas âncora, cores) permaneça centralizado
   em forma.c, respeitando o principio de encapsulamento.
 */
Forma* circulo_cria(int id, double x, double y, double r,
                    char* cor_borda, char* cor_preench) {
    return forma_cria_circulo(id, x, y, r, cor_borda, cor_preench);
}

/* o acesso aos dados internos específicos (raio) eh feito unicamente 
   através dos getters da struct genérica Forma, escondendo do utilizador 
   a implementação da união (union) contida em forma.c.
 */
double circulo_get_raio(Forma* f) {
    return forma_get_raio(f);
}

/* calcula a área do círculo usando a constante Pi com alta precisão.
 */
double circulo_get_area(Forma* f) {
    double r = forma_get_raio(f);
    return 3.14159265358979 * r * r;
}

/*
 * verifica se um ponto (px, py) está dentro do círculo usando a 
   equação da circunferência. 
 * para poupar memoria: em vez de calcular a distância real usando a raiz 
   quadrada (sqrt) — que eh uma operação computacionalmente custosa —,
   calculamos a distancia ao quadrado e comparamos com o raio ao quadrado
   (dx^2 + dy^2 <= r^2). o resultado logic eh matematicamente identico
   e poupa ciclos de processador.
 */
int circulo_contem_ponto(Forma* f, double px, double py) {
    double cx = forma_get_x(f);
    double cy = forma_get_y(f);
    double r  = forma_get_raio(f);
    
    double dx = px - cx;
    double dy = py - cy;
    
    return (dx*dx + dy*dy) <= (r*r);
}