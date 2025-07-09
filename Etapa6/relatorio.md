# Relatório da Etapa 6

- Compiladores 2025/1 - Professor Marcelo Johann
- Nathan Alonso Guimarães - 334437

## Configuração do Ambiente

O projeto foi desenvolvido e testado em um ambiente Linux horas com um processador AMD Ryzen 7 5800X e horas em um processador AMD Ryzen 9 9900X (ambos seguem a ISA `x86_64`).

## Detalhes da Implementação

Os tipos de dados `int` e `boolean` foram implementados completamente, com todas as funcionalidades da linguagem 2025++1, tendo suporte à vetores, operações lógicas e aritméticas, controle de fluxo, entrada/saída e chamadas de função. Já os tipos de dados `real` e `byte` foram deixados de lado. Há também suporte completo para `string` e `char`.

## Compilação do Projeto

Rodando o comando `make` ou `make etapa6`, o projeto foi compilado. Utilizando o comando `time make`, o tempo de compilação foi medido, resultando em aproximadamente 3 segundos:

``` bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ time make
bison -d -g -v --html parser.ypp
flex -o lex.yy.cpp scanner.l 
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o lex.yy.o lex.yy.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o parser.tab.o parser.tab.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o ast.o ast.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o symbol.o symbol.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o verifications.o verifications.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o tac.o tac.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o asm.o asm.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o main.o main.cpp
g++ lex.yy.o parser.tab.o ast.o symbol.o verifications.o tac.o asm.o main.o -o etapa6

real    0m3.034s
user    0m2.604s
sys     0m0.429s
```

Sendo o arquivo `asm.cpp` o mais demorado a se compilar. Note a compilação de todos os códigos-fonte com as flags `-Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG` e que nenhum erro ou aviso foi gerado durante toda a compilação.

## Testes do Projeto

Os testes foram automatizados utilizando o comando `make test`. O comando roda o executável `etapa6` (compilado com o comando anterior `make`) com um dado programa de teste e gera, no diretório `output/`, arquivos de texto com as estruturas intermediárias (AST, Tabela de Símbolos, TAC e Código Assembly) geradas ao longo do processo de compilação. Após isso, o comando compila o código Assembly gerado e executa o programa. O comando `make test` executa apenas um teste por vez.

Os códigos de teste estão localizados no diretório `tests/`. O código em Assembly é compilado com o comando `gcc -o output/<nome_do_arquivo>.out output/<nome_do_arquivo>.s`.

### Teste 1: Entrada/Saída no Arquivo `tests/io.2025++1`

O código é um exemplo simples de entrada e saída, onde o programa fica constantemente lendo um número do usuário através do comando `read` e imprimindo este número na tela com o comando `print`, até que o usuário digite `4` para sair do loop. Para testar a leitura e escrita de valores `real`, como não há suporte a controle de fluxo para este tipo, o programa apenas lê um valor e o imprime na tela. A execução se deu como:

```bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ make test FILE=tests/io.2025++1 
The file had 22 lines in total.
- Symbol table saved to file "output/io_symbol_table.txt".
- AST structure saved to file "output/io_AST.txt".
- Decompiled code saved to file "output/io_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/io_TAC.txt".
- Assembly code saved to file "output/io.s".

Generating binary code:
- Binary code saved at output/io.out

Running the program:
Entering an infinite loop to read integers, please enter 4 to exit: 
Enter an integer: 1
Read value: 1
Enter an integer: 2
Read value: 2
Enter an integer: 3
Read value: 3
Enter an integer: 4
Read value: 4
Exited the loop.

Enter a real number: 0.77
Read value: 0.77
Exited the loop.
- Test completed successfully for io.
```

### Teste 2: Operações Lógicas e Controle de Fluxo no Arquivo `tests/logic.2025++1`

O código é um exemplo de operações lógicas com números inteiros, onde o programa imprime o resultado de algumas operações lógicas (AND, OR, NOT) entre eles e também utiliza estruturas de controle de fluxo (if, while) com operações de comparação (LE, GE, etc.). A execução se deu como:

```bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ make test FILE=tests/logic.2025++1 
The file had 74 lines in total.
- Symbol table saved to file "output/logic_symbol_table.txt".
- AST structure saved to file "output/logic_AST.txt".
- Decompiled code saved to file "output/logic_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/logic_TAC.txt".
- Assembly code saved to file "output/logic.s".

Generating binary code:
- Binary code saved at output/logic.out

Running the program:
Testando operações lógicas em estruturas de controle:
a > b é verdadeiro
a <= b é falso
a >= b é verdadeiro
ambos a e b são positivos
a > 10 E b > 0
a > 10 OU b > 0 é verdadeiro
nenhum é negativo
Contando com MOD:
0 é par
1 é ímpar
2 é par
3 é ímpar
4 é par
- Test completed successfully for logic.
```

### Teste 3: Vetores no Arquivo `tests/vector.2025++1`

O código é um exemplo de manipulação de vetores, onde o programa cria um vetor de inteiros e outro de reais, inicializa-os com valores e imprime os valores dos vetores. Também é testado o acesso e soma com os valores destes vetores. A execução se deu como:

```bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ make test FILE=tests/vectors.2025++1 
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11   -c -o asm.o asm.cpp
g++ lex.yy.o parser.tab.o ast.o symbol.o verifications.o tac.o asm.o main.o -o etapa6
The file had 41 lines in total.
- Symbol table saved to file "output/vectors_symbol_table.txt".
- AST structure saved to file "output/vectors_AST.txt".
- Decompiled code saved to file "output/vectors_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/vectors_TAC.txt".
- Assembly code saved to file "output/vectors.s".

Generating binary code:
- Binary code saved at output/vectors.out

Running the program:
Testing int vector adressing: 
1 2 3 4 5 6 7 8 9 10 
Testing real vector adressing: 
0.00 1.00 0.75 0.80 0.83 0.86 0.88 0.89 9.00 0.09 
Testing int vector sum: 
Final sum: 55
Testing real vector sum: 
Final sum: 15.76
- Test completed successfully for vectors.
```

Por algum motivo, a primeira posição do vetor real não pode ser lida. Tentei solucionar isso, mas não consegui descobrir o que estava acontecendo. O código de teste está no arquivo `tests/vectors.2025++1` e o Assembly correspondente está no arquivo `output/vectors.s`.

### Teste 4: Funções no Arquivo `tests/functions.2025++1`

O código é um exemplo de funções, onde o programa define uma função que lê dois inteiros e imprime a soma, multiplicação e divisão deles. A execução se deu como:

```bash

nathan@nathan-casa:~/Documents/compilers/Etapa6$ make test FILE=tests/functions.2025++1 
The file had 33 lines in total.
- AST structure saved to file "output/functions_AST.txt".
- Decompiled code saved to file "output/functions_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- Symbol table saved to file "output/functions_symbol_table.txt".
- TAC list saved to file "output/functions_TAC.txt".
- Assembly code saved to file "output/functions.s".

Generating binary code:
- Binary code saved at output/functions.out

Running the program:
Valor para var1: 10
Valor para var2: 2
Soma de var1 e var2: 12
Multiplicacao de var1 e var2: 20
Divisao de var1 por var2: 5
- Test completed successfully for functions.
```
