# Relatório da Etapa 6

- Compiladores 2025/1 - Professor Marcelo Johann
- Nathan Alonso Guimarães - 334437

## Configuração do Ambiente

O projeto foi desenvolvido e testado em um ambiente Linux horas com um processador AMD Ryzen 7 5800X e horas em um processador AMD Ryzen 9 9900X (ambos seguem a ISA `x86_64`).

## Detalhes da Implementação

Os tipos de dados `int` e `boolean` foram implementados completamente, com todas as funcionalidades da linguagem 2025++1, tendo suporte à vetores, operações lógicas e aritméticas, controle de fluxo, entrada/saída e chamadas de função. Já os tipos de dados `real` e `byte` foram deixados de lado. Há também suporte completo para `string` e `char`.

## Compilação do Projeto

Rodando o comando `make` ou `make etapa6`, o projeto foi compilado. Utilizando o comando `time make` em um computador com o processador AMD Ryzen 7 5800X, o tempo de compilação foi medido, resultando em aproximadamente 2.5 segundos:

``` bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ time make
bison -d -g -v --html parser.ypp
flex -o lex.yy.cpp scanner.l 
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o lex.yy.o lex.yy.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o parser.tab.o parser.tab.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o symbol.o symbol.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o verifications.o verifications.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o tac.o tac.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o asm.o asm.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o main.o main.cpp
g++ lex.yy.o parser.tab.o ast.o symbol.o verifications.o tac.o asm.o main.o -o etapa6

real    0m2.528s
user    0m2.159s
sys     0m0.370s
```

Sendo o arquivo `asm.cpp` o mais demorado a se compilar. Note a compilação de todos os códigos-fonte com as flags `-Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG` e que nenhum erro ou aviso foi gerado durante toda a compilação.

## Testes do Projeto

Os testes foram automatizados utilizando o comando `make test`. O comando roda o executável `etapa6` (compilado com o comando anterior `make`) com um dado programa de teste e gera, no diretório `output/`, arquivos de texto detalhados com as estruturas intermediárias (AST, Tabela de Símbolos, TAC e Código Assembly) geradas ao longo do processo de compilação. Após isso, o comando compila o código Assembly gerado e executa o programa. O comando `make test` executa apenas um teste por vez.

Os códigos de teste estão localizados no diretório `tests/`. O código em Assembly é compilado com o comando `gcc -o output/<nome_do_arquivo>.out output/<nome_do_arquivo>.s`.

### Teste 1: Entrada/Saída no Arquivo `tests/io.2025++1`

O código é um exemplo simples de entrada e saída, onde o programa fica constantemente lendo um número do usuário através do comando `read` e imprimindo este número na tela com o comando `print`, até que o usuário digite `4` para sair do loop. A execução se deu como:

```bash
nathan@626679:~/Documents/compilers/Etapa6$ make test FILE=tests/io.2025++1 
The file had 15 lines in total.
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
Enter an integer: 0
Read value: 0
Enter an integer: 1
Read value: 1
Enter an integer: -3
Read value: -3
Enter an integer: 7
Read value: 7
Enter an integer: 4
Read value: 4
Exited the loop.
- Test completed successfully for io.
```

### Teste 2: Operações Lógicas e Controle de Fluxo no Arquivo `tests/logic.2025++1`

O código é um exemplo de operações lógicas com números inteiros e booleanos, onde o programa imprime o resultado de algumas operações lógicas (AND, OR, NOT) entre eles e também utiliza estruturas de controle de fluxo (if, while) com operações de comparação (LE, GE, etc.). A execução se deu como:

```bash
nathan@626679:~/Documents/compilers/Etapa6$ make test FILE=tests/logic.2025++1 
The file had 82 lines in total.
- Symbol table saved to file "output/logic_symbol_table.txt".
- AST structure saved to file "output/logic_AST.txt".
- Decompiled code saved to file "output/logic_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/logic_TAC.txt".
- Assembly code saved to file "output/logic.s".

Generating binary code:
- Binary code saved at output/logic.out

Running the program:
Enter a value for x: -2
Enter a value for y: 7
Enter a value for counter: 3

Comparison Tests:
x > y: FALSE
x < y: TRUE
x >= y: FALSE
x <= y: TRUE
x == y: FALSE
x != y: TRUE

Logical Tests:
Both positive: FALSE
At least one positive: TRUE
Both negative: FALSE

MOD Tests:
x is even
y not divisible by 3

Loop tests should repeat 3 times.
While-Do Test:
Counter: 3
Counter: 2
Counter: 1

Do-While Test:
Counter: 3
Counter: 2
Counter: 1

Nested Conditions:
x <= 5 BUT y < 10

If Without Else Test:
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

O código é um exemplo de funções, no qual o programa faz diversas operações com vetores, como calcular a soma, encontrar o máximo e mínimo, criar um vetor dobrado e inverter a ordem dos elementos. A execução se deu como:

```bash
nathan@nathan-casa:~/Documents/compilers/Etapa6$ make test FILE=tests/vectors.2025++1 
The file had 70 lines in total.
- AST structure saved to file "output/vectors_AST.txt".
- Decompiled code saved to file "output/vectors_decompiled.txt".
Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/vectors_TAC.txt".
- Symbol table saved to file "output/vectors_symbol_table.txt".
- Assembly code saved to file "output/vectors.s".

Generating binary code:
- Binary code saved at output/vectors.out

Running the program:
Original vector: 1 2 3 4 5 6 7 8 9 10 
Calculating sum...
Sum: 55
Finding max and min...
Max: 10, Min: 1
Creating doubled vector...
Doubled vector: 2 4 6 8 10 12 14 16 18 20 
Reverse order: 10 9 8 7 6 5 4 3 2 1 
- Test completed successfully for vectors.
```
