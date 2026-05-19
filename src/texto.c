#include "texto.h"
#include "forma.h"

/*
 * comprimento euclidiano: sqrt((x2-x1)^2 + (y2-y1)^2)
 * x1,y1 estao na ancora (forma_get_x, forma_get_y)
 * x2,y2 estao nos dados especificos da linha
 */
Forma* texto_cria(int id, double x, double y,
                  char* cor_borda, char* cor_preench,
                  char ancora_tipo, char* conteudo) {
    return forma_cria_texto(id, x, y, cor_borda, cor_preench,
                            ancora_tipo, conteudo);
}
