PROJ_NAME = ted
CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -ggdb -O0 -Wall
LDFLAGS = -O0 -lm

SRC   = .
TST   = ../tst
UNITY = ../unity
INC   = -I$(SRC) -I$(UNITY)

# Adicionado arvore.o aos objetos principais
OBJS = forma.o \
       circulo.o \
       retangulo.o \
       linha.o \
       texto.o \
       fila.o \
       lista.o \
       poligono.o \
       arvore.o \
       svg.o

ted: $(OBJS) main.o
	$(CC) $(LDFLAGS) -o $(PROJ_NAME) $(OBJS) main.o -lm

main.o: main.c
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

forma.o: forma.c forma.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

circulo.o: circulo.c circulo.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

retangulo.o: retangulo.c retangulo.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

linha.o: linha.c linha.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

texto.o: texto.c texto.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

fila.o: fila.c fila.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

lista.o: lista.c lista.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

poligono.o: poligono.c poligono.h fila.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

# Regra de compilação do módulo arvore
arvore.o: arvore.c arvore.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

svg.o: svg.c svg.h
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

# Regras de compilação dos testes unitários
$(TST)/t_fila: $(TST)/t_fila.c fila.c $(UNITY)/unity.c
	$(CC) $(CFLAGS) $(INC) $(UNITY)/unity.c fila.c $(TST)/t_fila.c -o $(TST)/t_fila

$(TST)/t_lista: $(TST)/t_lista.c lista.c $(UNITY)/unity.c
	$(CC) $(CFLAGS) $(INC) $(UNITY)/unity.c lista.c $(TST)/t_lista.c -o $(TST)/t_lista

$(TST)/t_forma: $(TST)/t_forma.c forma.c $(UNITY)/unity.c
	$(CC) $(CFLAGS) $(INC) $(UNITY)/unity.c forma.c $(TST)/t_forma.c -o $(TST)/t_forma

$(TST)/t_poligono: $(TST)/t_poligono.c poligono.c fila.c $(UNITY)/unity.c
	$(CC) $(CFLAGS) $(INC) -DUNITY_INCLUDE_DOUBLE $(UNITY)/unity.c fila.c poligono.c $(TST)/t_poligono.c -o $(TST)/t_poligono

# Regra de compilação do teste da árvore
$(TST)/t_arvore: $(TST)/t_arvore.c arvore.c $(UNITY)/unity.c
	$(CC) $(CFLAGS) $(INC) $(UNITY)/unity.c arvore.c $(TST)/t_arvore.c -o $(TST)/t_arvore

# Alvos para execução individual dos testes
tst_fila: $(TST)/t_fila
	./$(TST)/t_fila

tst_lista: $(TST)/t_lista
	./$(TST)/t_lista

tst_forma: $(TST)/t_forma
	./$(TST)/t_forma

tst_poligono: $(TST)/t_poligono
	./$(TST)/t_poligono

# Alvo para executar o teste da árvore
tst_arvore: $(TST)/t_arvore
	./$(TST)/t_arvore

# tst_arvore incluído no tstall
tstall: tst_fila tst_lista tst_forma tst_poligono tst_arvore

clean:
	rm -f *.o $(PROJ_NAME)
	rm -f $(TST)/t_fila $(TST)/t_lista $(TST)/t_forma $(TST)/t_poligono $(TST)/t_arvore

VG = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s

# t_arvore incluído na checagem do Valgrind
vtest: tstall
	$(VG) ./$(TST)/t_fila
	$(VG) ./$(TST)/t_lista
	$(VG) ./$(TST)/t_forma
	$(VG) ./$(TST)/t_poligono
	$(VG) ./$(TST)/t_arvore

vted: ted
	$(VG) ./$(PROJ_NAME) -e . -f teste.geo -o .

.PHONY: ted tstall tst_fila tst_lista tst_forma tst_poligono tst_arvore clean vtest vted