# Relatório da Etapa 7

- Compiladores 2025/1 - Professor Marcelo Johann
- Nathan Alonso Guimarães - 334437

## Configuração do Ambiente

O projeto foi desenvolvido e testado em um ambiente Linux horas com um processador AMD Ryzen 7 5800X e horas em um processador AMD Ryzen 9 9900X (ambos seguem a ISA `x86_64`).

## Compilação do Projeto

Rodando o comando `make` ou `make etapa7`, o projeto foi compilado. Utilizando o comando `time make` em um computador com o processador AMD Ryzen 9 9900X, o tempo de compilação foi medido, resultando em aproximadamente 1.7 segundo:

``` bash
nathan@626679:~/Documents/compilers/Etapa7$ time make
bison -d parser.ypp
flex -o lex.yy.cpp scanner.l 
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o lex.yy.o lex.yy.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o parser.tab.o parser.tab.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o ast.o ast.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o symbol.o symbol.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o verifications.o verifications.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o tac.o tac.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o asm.o asm.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o optimizer.o optimizer.cpp
g++ -Wall -Wextra -pedantic -Wconversion -std=c++11 -g -DDEBUG   -c -o main.o main.cpp
g++ lex.yy.o parser.tab.o ast.o symbol.o verifications.o tac.o asm.o optimizer.o main.o -o etapa7

real    0m1.773s
user    0m1.577s
sys     0m0.197s
```

Há outros 2 comandos disponíveis no Makefile:
    - `make test FILE=<nome_do_arquivo>`: compila o arquivo especificado, gera os arquivos de saída com as estruturas intermédiarias e executa o teste.
    - `make optimize FILE=<nome_do_arquivo>`: compila o arquivo especificado, gera os arquivos de saída com as estruturas intermediárias e executa o otimizador TAC, gerando o código otimizado.

## Detalhes da Implementação - Recuperação de Erros

A recuperação de erros foi implementada utilizando o token reservado `error` no arquivo `parser.ypp`, no qual foram adicionadas regras para encontrar pontos de sincronização e continuar a análise sintática mesmo após encontrar erros. Os seguintes pontos de sincronização foram definidos:

### 1. Declarações Globais

- **Declaração global inválida**: recupera erros quando uma declaração global não segue a sintaxe esperada (variável, vetor ou função), sincronizando no próximo token válido de declaração.

### 2. Declaração de Variáveis

- **Inicialização inválida**: captura erros quando o valor de inicialização de uma variável não é um literal válido (inteiro, caractere ou real).

### 3. Declaração de Vetores

- **Tamanho de vetor inválido**: recupera erros quando o tamanho do vetor não é um literal inteiro.
- **Inicialização de vetor inválida**: captura erros na inicialização do vetor quando não há uma lista válida de literais separados por vírgulas.

### 4. Declaração de Funções

- **Lista de parâmetros inválida**: recupera erros quando os parâmetros formais não seguem o padrão "tipo identificador" separados por vírgulas.
- **Corpo da função inválido**: captura erros quando o bloco de comandos da função não está corretamente delimitado por chaves.

### 5. Blocos de Comandos

- **Conteúdo de bloco inválido**: recupera erros dentro de blocos de comandos quando há instruções malformadas, sincronizando nas chaves de fechamento.
- **Sintaxe de declaração inválida**: captura erros em declarações mal formadas dentro de listas de comandos, sincronizando no próximo ponto-e-vírgula.

### 6. Comandos e Instruções

- **Ponto-e-vírgula ausente**: recupera erros quando comandos de atribuição, leitura, impressão ou retorno não terminam com ponto-e-vírgula.

### 7. Atribuições

- **Expressão de atribuição inválida**: captura erros quando a expressão do lado direito da atribuição não é válida.
- **Índice de vetor inválido**: recupera erros quando o índice de acesso a vetores não é uma expressão válida.
- **Atribuição a vetor inválida**: captura erros na atribuição de valores a elementos de vetores.

### 8. Comandos de Entrada e Saída

- **Comando print inválido**: recupera erros quando o comando print não contém literais de string ou expressões válidas.

### 9. Comando de Retorno

- **Expressão de retorno inválida**: captura erros quando a expressão após a palavra-chave `return` não é válida.

### 10. Expressões

- **Expressão entre parênteses inválida**: recupera erros quando há expressões malformadas dentro de parênteses.
- **Acesso a vetor inválido**: captura erros quando a expressão de índice para acesso a vetores não é válida.

### 11. Chamadas de Função

- **Argumentos de função inválidos**: recupera erros quando os argumentos passados para uma função não formam uma lista válida de expressões separadas por vírgulas.

### 12. Controle de Fluxo

- **Condição if inválida**: captura erros quando a expressão condicional do `if` não é uma expressão booleana válida.
- **Condição if-else inválida**: recupera erros na condição de estruturas `if-else`.
- **Condição while inválida**: captura erros quando a expressão condicional entre `while` e `do` não é válida.
- **Condição do-while inválida**: recupera erros quando a expressão condicional após `while` em estruturas `do-while` não é válida.

Cada ponto de sincronização emite uma mensagem de erro específica indicando o tipo de erro encontrado e a linha onde ocorreu, permitindo que o compilador continue a análise e identifique múltiplos erros em uma única execução.

### Teste de Recuperação de Erros

Foi criado o arquivo `tests/error_recovery.2025++1` contendo um conjunto abrangente de testes que demonstra a funcionalidade de recuperação de erros sintáticos. O arquivo de teste inclui exemplos específicos de cada tipo de erro que pode ser recuperado pelo parser, organizados em 14 categorias principais que abrangem desde declarações globais malformadas até estruturas de controle de fluxo inválidas.

O teste captura **21 dos 25 tipos** de erros sintáticos implementados no parser, incluindo erros como inicializações inválidas de variáveis (`int x = ;`), tamanhos de vetores incorretos (`int arr[xyz];`), parâmetros de função malformados, expressões entre parênteses vazias (`w = ( );`), e condições de controle de fluxo inválidas.

A execução do teste retornou:

``` bash
nathan@626679:~/Documents/compilers/Etapa7$ make test FILE=tests/error_recovery.2025++1 
Syntax error at line 9: Invalid variable initialization - expected literal value (integer, character, or real).
Syntax error at line 13: Invalid vector size specification - expected integer literal for array size.
Syntax error at line 15: Invalid vector initialization - expected comma-separated list of literal values.
Syntax error at line 21: Invalid function parameter list - expected type-identifier pairs separated by commas.
Syntax error at line 22: Invalid function body - expected command block enclosed in braces '{}'.
Syntax error at line 36: Invalid statement syntax - check for missing operators, identifiers, or semicolons.
Syntax error at line 43: Missing semicolon ';' after assignment statement.
Syntax error at line 44: Missing semicolon ';' after print statement.
Syntax error at line 45: Missing semicolon ';' after read statement.
Syntax error at line 48: Missing semicolon ';' after return statement.
Syntax error at line 49: Invalid assignment expression - expected valid expression on right side of '='.
Syntax error at line 51: Invalid array assignment expression - expected valid expression on right side of '='.
Syntax error at line 55: Invalid print statement - expected string literals or expressions to print.
Syntax error at line 59: Invalid return statement - expected valid expression after 'return' keyword.
Syntax error at line 62: Invalid parenthesized expression - expected valid expression inside parentheses.
Syntax error at line 63: Invalid array access - expected valid index expression inside square brackets.
Syntax error at line 75: Invalid if condition - expected boolean expression inside parentheses.
Syntax error at line 79: Invalid if-else condition - expected boolean expression inside parentheses.
Syntax error at line 83: Invalid while condition - expected boolean expression between 'while' and 'do' keywords.
Syntax error at line 87: Invalid do-while condition - expected boolean expression after 'while' keyword.
Syntax error at line 99: Invalid statement syntax - check for missing operators, identifiers, or semicolons.
Syntax error at line 99: Invalid statement syntax - check for missing operators, identifiers, or semicolons.
Syntax error at line 99: Invalid statement syntax - check for missing operators, identifiers, or semicolons.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
Syntax error at line 99: Invalid global declaration - expected variable, vector, or function declaration.
The file had 99 lines in total.
- AST structure saved to file "output/error_recovery_AST.txt".
- Decompiled code saved to file "output/error_recovery_decompiled.txt".
Semantic error at line 50: Identifier "bad_index" is undeclared.
Syntax analysis completed with 19 error(s).
Semantic analysis completed with 1 error(s).
- Error: Compilation failed.
```

## Detalhes da Implementação - Otimizações

Nos arquivos `optimizer.cpp` e `optimizer.hpp`, foram implementadas duas otimizações: **Constant Folding** e **Dead Code Elimination**.

### Constant Folding

O otimizador identifica operações aritméticas e lógicas cujos operandos são todos literais numéricos escalares (dados declarados diretamente no arquivo, e não lidos pelo usuário em tempo de execução) e as substitui por um único valor constante. Por exemplo, `soma = 2 + 3 * 4` é simplificado para `soma = 14`. A implementação utiliza:

- **`isNumericLiteral()`**: verifica se um símbolo é um literal numérico (tipos `LIT_INT` ou `LIT_REAL`)
- **`computeConstantExpression()`**: executa as operações aritméticas (`ADD`, `SUB`, `MUL`, `DIV`, `MOD`) e lógicas (`LT`, `GT`, `EQ`, `AND`, `OR`, etc.)
- **`createConstantSymbol()`**: cria um novo símbolo com o valor computado

### Dead Code Elimination

Remove instruções `MOVE` que atribuem constantes a temporários não utilizados posteriormente no código. A implementação inclui:

- **`isSymbolUsed()`**: verifica se um símbolo é referenciado em instruções subsequentes
- **`substituteSymbolUses()`**: substitui todas as ocorrências de um símbolo por outro

### Propagação de Constantes

Combina as duas otimizações anteriores substituindo usos de temporários definidos por `MOVE` com constantes pelos valores originais, permitindo posterior eliminação do `MOVE` e constant folding das operações que usam esses temporários.

### Algoritmo de Otimização

O otimizador utiliza um loop iterativo que executa múltiplos passes sobre o código TAC até que nenhuma otimização adicional seja possível. Em cada passe, percorre sequencialmente todas as instruções TAC aplicando as três otimizações (dead code elimination, propagação de constantes e constant folding). O loop continua enquanto houver modificações (`hasOptimizations = true`), garantindo aplicação recursiva e completa das transformações.

Para prevenir loops infinitos causados por erros de implementação ou casos patológicos, foi implementado um limite de segurança de 1000 passes, após o qual o otimizador emite um aviso e para a execução. Este limite é suficientemente alto para permitir otimização completa de programas complexos, mas previne travamentos do compilador.

### Teste para as otimizações

Para o arquivo `tests/constant_folding.2025++1`, a expressão `(((02 * 03) / 01) + 04 - 05 + 06 - 07 + 08 - 09 + 10)` foi completamente simplificada para a constante `31` em apenas 2 passes, eliminando 18 operações intermediárias. Estas mesmas 18 operações seriam repetidas em cada execução do loop no programa não otimizado, o que resulta em uma redução de aproximadamente `(1000000000 - 1) * 18` operações.

Primeiro, compilei o código para gerar ambos os arquivos de saída, o original e o otimizado:

``` bash
nathan@626679:~/Documents/compilers/Etapa7$ make optimize FILE=tests/constant_folding.2025++1 
The file had 17 lines in total.
Syntax analysis completed successfully - no errors found.
- AST structure saved to file "output/constant_folding_AST.txt".
- Decompiled code saved to file "output/constant_folding_decompiled.txt".

Semantic analysis completed successfully - no errors found.
- TAC list saved to file "output/constant_folding_TAC.txt".
- Symbol table saved to file "output/constant_folding_symbol_table.txt".
- Original assembly code saved to file "output/constant_folding.s".

Constant folding optimization completed: 18 expressions optimized in 2 passes.
- TAC optimization completed successfully and saved to "output/constant_folding_TAC_optimized.txt".
- Optimized symbol table saved to file "output/constant_folding_symbol_table_optimized.txt".
- Optimized assembly code saved to file "output/constant_folding_optimized.s".

Generating original binary code:
- Binary code saved at output/constant_folding.out
Generating optimized binary code:
- Binary code saved at output/constant_folding_optimized.out

Compilation completed successfully for tests/constant_folding.2025++1.
```

O teste da otimização foi executado em um processador muito potente e de lançamento recente, o que influencia os tempos de execução. Caso o teste seja rodado em um processador menos potente, a otimização deve reduzir percentualmente ainda mais o tempo de execução.

A execução do código original, não otimizado:

``` bash
nathan@626679:~/Documents/compilers/Etapa7$ time ./output/constant_folding.out 
Result of the loop: 935228928


real    0m1.843s
user    0m1.843s
sys     0m0.001s
```

A execução do código otimizado:

``` bash
nathan@626679:~/Documents/compilers/Etapa7$ time ./output/constant_folding.out 
Result of the loop: 935228928


real    0m0.565s
user    0m0.562s
sys     0m0.003s
```
