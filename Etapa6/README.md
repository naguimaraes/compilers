# Etapa 6 - Geração de Código Assembly x86

## Descrição

Esta etapa implementa a geração de código assembly x86 a partir das instruções TAC (Three-Address Code) geradas na Etapa 5. O gerador traduz cada tipo de instrução TAC em sequências equivalentes de instruções assembly x86.

## Arquivos Principais

- `asm.hpp` - Interface do gerador de assembly
- `asm.cpp` - Implementação do gerador de assembly
- `main.cpp` - Programa principal integrado com geração de assembly
- `Makefile` - Atualizado para incluir compilação do gerador de assembly

## Funcionalidades Implementadas

### Tipos de TAC Suportados

1. **Inicialização de Variáveis (INIT)**
   - Aloca espaço na stack e inicializa com valor
   - Exemplo: `int a = 10;` → `movl $10, -4(%ebp)`

2. **Atribuições (MOVE)**
   - Move valores entre variáveis e registradores
   - Exemplo: `a = b;` → `movl -8(%ebp), %eax; movl %eax, -4(%ebp)`

3. **Operações Aritméticas**
   - **ADD**: `addl src, dest`
   - **SUB**: `subl src, dest`
   - **MUL**: `imull src, dest`
   - **DIV**: `idivl divisor` (quociente em %eax)
   - **MOD**: `idivl divisor` (resto em %edx)

4. **Operações de Comparação**
   - **LT, GT, LE, GE, EQ, NE**: Geram código com comparação e saltos condicionais
   - Resultado em variável temporária (0 = falso, 1 = verdadeiro)

5. **Operações Lógicas**
   - **AND**: `andl src, dest`
   - **OR**: `orl src, dest`
   - **NOT**: Inversão lógica com comparação a zero

6. **Controle de Fluxo**
   - **LABEL**: Marcadores no código
   - **JUMP**: Salto incondicional `jmp label`
   - **IFZ**: Salto condicional se zero `je label`

7. **Funções**
   - **BEGINFUN**: Prólogo da função (configuração stack frame)
   - **ENDFUN**: Epílogo da função (restauração stack frame)
   - **CALL**: Chamada de função `call function_name`
   - **ARG**: Colocação de argumentos na stack `pushl arg`
   - **RET**: Retorno de função `movl value, %eax`

8. **Entrada/Saída**
   - **PRINT**: Chamada `printf` com formato apropriado
   - **READ**: Chamada `scanf` para leitura

9. **Vetores**
   - **VECWRITE**: Escrita em vetor usando endereçamento calculado
   - **VECREAD**: Leitura de vetor usando endereçamento calculado
   - **BEGINVEC/ENDVEC**: Delimitadores de declaração de vetor

## Características do Gerador

### Gerenciamento de Memória
- **Stack-based**: Todas as variáveis são alocadas na stack
- **Offset automático**: Calcula offsets automaticamente a partir de %ebp
- **Mapeamento**: Mantém tabela de localização de variáveis

### Convenções de Registradores
- **%eax**: Registrador principal para operações e valores de retorno
- **%ebx**: Registrador auxiliar para operações binárias
- **%ecx, %edx**: Usados conforme necessário (divisão usa %edx)
- **%ebp**: Base pointer para stack frame
- **%esp**: Stack pointer

### Formato de Saída
```assembly
.section .data
    int_format: .string "%d"
    char_format: .string "%c"
    # ... outros formatos

.section .text
.globl _start
_start:
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80

main:
    pushl %ebp
    movl %esp, %ebp
    # ... código da função
    movl %ebp, %esp
    popl %ebp
    ret
```

## Como Usar

### Compilação
```bash
make clean
make
```

### Execução
```bash
./etapa6 input.txt output.txt [assembly_output.s]
```

**Parâmetros:**
- `input.txt`: Arquivo fonte na linguagem 2025++1
- `output.txt`: Código descompilado (da AST)
- `assembly_output.s`: Arquivo assembly gerado (opcional, padrão: `output.s`)

### Exemplo de Uso
```bash
# Compilar programa simples
./etapa6 test_simple.txt test_simple_decompiled.txt test_simple.s

# Compilar exemplo completo
./etapa6 etapa6.txt etapa6_decompiled.txt etapa6.s
```

## Exemplo de Geração

### Código Fonte
```c
int a = 10;

int main() {
    print a;
    return 0;
}
```

### TACs Geradas
```
| INIT        | a           | 10          |             |
| BEGINFUN    |             | main        |             |
| PRINT       |             | a           |             |
| RET         |             | 0           |             |
| ENDFUN      |             | main        |             |
```

### Assembly Gerado
```assembly
.section .data
    int_format: .string "%d"
    # ... outros formatos

.section .text
.globl _start
_start:
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80

    # Initialize a = $10
    movl $10, -4(%ebp)
main:
    # Function main prologue
    pushl %ebp
    movl %esp, %ebp
    # Print -4(%ebp)
    pushl -4(%ebp)
    pushl $int_format
    call printf
    addl $8, %esp
    # Return $0
    movl $0, %eax
    # Function main epilogue
    movl %ebp, %esp
    popl %ebp
    ret
```

## Limitações Conhecidas

1. **Sem otimizações**: O código gerado é funcional mas não otimizado
2. **Registradores limitados**: Usa principalmente %eax e %ebx
3. **Sem verificação de tipos**: Assume que verificações semânticas já foram feitas
4. **Assembly x86-32**: Focado em arquitetura 32-bit
5. **Sistema de chamadas Linux**: Usa convenções do Linux para syscalls

## Arquitetura do Código

### Classe AssemblyGenerator
- **Construtor**: Inicializa com stream de saída
- **generateAssembly()**: Função principal que processa lista de TACs
- **handle*()**: Funções específicas para cada tipo de TAC
- **Utilities**: Funções auxiliares para alocação e manipulação

### Fluxo de Geração
1. **Coleta informações**: Percorre TACs para identificar vetores
2. **Seção .data**: Gera strings de formato e literais
3. **Seção .text**: Gera código de inicialização
4. **Processamento TACs**: Converte cada TAC em assembly
5. **Finalização**: Fecha arquivo e reporta sucesso

## Testes

Para validar o gerador, teste com:
- Programas simples (variáveis, prints, retornos)
- Operações aritméticas e lógicas
- Estruturas de controle (if, while, do-while)
- Funções com parâmetros e retorno
- Vetores com inicialização e acesso
- Programas complexos como `etapa6.txt`

## Integração

O gerador de assembly está totalmente integrado ao fluxo do compilador:
1. Análise léxica e sintática
2. Construção da AST
3. Verificações semânticas
4. Geração de TACs
5. **Geração de Assembly** ← Nova etapa
6. Saída do código assembly executável

A geração de assembly é chamada automaticamente após a geração bem-sucedida das TACs.
