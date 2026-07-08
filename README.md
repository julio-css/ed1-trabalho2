## 📋 Descrição

Trabalho prático da disciplina **Estrutura de Dados I** (ED1) - 2026.1.

Implementação de um sistema para processamento de formas geométricas (retângulos, círculos, linhas, textos e polígonos) com suporte a:

- **Árvore Binária de Busca (ABB)** para armazenamento e consulta eficiente
- **Comandos interativos** para manipulação das formas (seleção, clonagem, movimentação, remoção)
- **Animações** do processo de ordenação com geração de frames SVG
- **Algoritmos de ordenação** com visualização passo-a-passo

## 🎯 Funcionalidades

### 📁 Arquivo `.geo` (Entrada)
- Leitura de formas geométricas (círculos, retângulos, linhas, textos)
- Polígonos definidos por pontos de âncora
- Estilos de texto (fonte, peso, tamanho)

### 📄 Arquivo `.qry` (Consultas)
| Comando | Descrição |
|---------|-----------|
| `sel` | Seleciona formas dentro de uma região |
| `find` | Ordena as formas selecionadas e posiciona os k menores |
| `findrm` | Similar ao find, mas remove os elementos restantes |
| `cm` | Clona formas dentro de uma região |
| `mcs` | Move e recolore formas selecionadas |
| `mc` | Recolore formas selecionadas |
| `dels` | Remove formas selecionadas |
| `inp` | Insere ponto em um polígono |
| `rmp` | Remove ponto de um polígono |
| `clp` | Limpa um polígono |
| `pol` | Gera um polígono a partir de pontos |

### 🎬 Animações
- Geração de frames SVG a cada passo significativo da ordenação
- Cache de fundo para otimização de performance
- Visualização do retângulo de seleção e fileira de destino
- Destaque para elementos comparados/trocados

## 🚀 Como Executar

### Compilação

``` bash
make ted
```


Execução
```bash
./ted -e caminho/entrada -f arquivo.geo -o caminho/saida [-q arquivo.qry]
```
Parâmetros:
-e path : Diretório de entrada (padrão: .)

-f arq : Arquivo .geo (obrigatório)

-o dir : Diretório de saída (obrigatório)

-q arq : Arquivo .qry (opcional)

Exemplo:
``` bash
./ted -e testes/entrada -f cidade.geo -o saida -q consultas.qry
```
Gerando Vídeo da Animação
```bash
ffmpeg -framerate 30 -pattern_type glob -i 'saida/*.svg' -c:v libx264 -pix_fmt yuv420p animacao.mp4
```

## 📁 Estrutura do Projeto

```text
.
├──/src
    ├── Makefile             # Configuração de compilação
    ├── arvore.c/h           # Árvore Binária de Busca
    ├── circulo.c/h          # Círculo
    ├── retangulo.c/h        # Retângulo
    ├── linha.c/h            # Linha
    ├── texto.c/h            # Texto
    ├── poligono.c/h         # Polígono
    ├── forma.c/h            # Tipo base polimórfico
    ├── geo.c/h              # Leitura .geo e comparadores
    ├── qry.c/h              # Processamento .qry
    ├── svg.c/h              # Geração de SVG
    ├── sort.c/h             # Algoritmos de ordenação
    ├── lista.c/h            # Lista duplamente encadeada
    ├── fila.c/h             # Fila circular estática
    ├── main.c               # Ponto de entrada

├──/tst
    ├── t_arvore.c
    ├── t_fila.c
    ├── t_forma.c
    ├── t_geo.c
    ├── t_lista.c
    ├── t_poligono.c
    ├── t_qry.c
    ├── t_sort.c

├── /unity
    ├── unity.c
    ├── unity.h
    ├── unity_internals.c 
/

a : Área crescente

w : Largura crescente

h : Altura crescente

c : Cor (alfabética)
```

## 📖 Exemplos de Uso
1. Seleção e Ordenação
```text
sel 10.0 20.0 100.0 80.0
find 5 ss a 0.0 500.0 15.0
```
2. Clonagem e Movimentação
```text
cm 10.0 20.0 100.0 80.0 200.0 50.0
mcs 50.0 30.0 #ff0000 #00ff00
```
3. Polígonos
```text
inp 1 10 10 15
inp 1 20 20 16
inp 1 30 10 17
pol 1 100 #ff0000 #0000ff
```
## 🛠️ Tecnologias
Linguagem: C (padrão C99)

Compilador: GCC com flags -Wall -fstack-protector-all -ggdb

Framework de Testes: Unity

Build: Makefile

## 👥 Autor
Julio Cesar da Silva Santos- julio-css

## ⚠️ Observações
Todos os arquivos .h estão documentados no padrão Doxygen

A struct Forma é opaca (definida apenas no .c) para encapsulamento

O programa usa open_memstream (POSIX) para cache de SVG

Polígonos requerem no mínimo 3 pontos para serem criados

A animação da ordenação gera frames SVG que podem ser convertidos para vídeo com ffmpeg
