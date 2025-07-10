# Generated x86 Assembly Code from 2025++1 code
# Compiler made by Nathan Guimaraes (334437)

.section .data
	int_format: .string "%d"
	char_format: .string "%c"
	string_format: .string "%s"
	newline: .string "\n"
	scanf_int: .string "%d"
	scanf_char: .string " %c"
	var1: .long 0
	var2: .long 0
	str_0: .string "Value for var1: "
	str_1: .string "Value for var2: "
	str_2: .string "Sum of var1 and var2: "
	str_3: .string "\n"
	str_4: .string "Multiplication of var1 and var2: "
	str_5: .string "Subtraction, in modulo, of var1 and var2: "
	str_6: .string "Remainder of division of var2 by var1: "
	str_7: .string "Division by zero is not allowed.\n"
	str_8: .string "Division of var2 by var1: "
	str_9: .string "Remainder of division of var1 by var2: "
	str_10: .string "Division of var1 by var2: "

.section .text
.globl main
.extern printf
.extern scanf

main:
	# Function main prologue
	pushq %rbp
	movq %rsp, %rbp
	# Print "Value for var1: "
	leaq str_0(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into var1
	leaq var1(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "Value for var2: "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into var2
	leaq var2(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "Sum of var1 and var2: "
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function sum
	movl var1(%rip), %edi
	movl var2(%rip), %esi
	call sum
	movl %eax, -4(%rbp)
	# Print -4(%rbp)
	movl -4(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Multiplication of var1 and var2: "
	leaq str_4(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function multiply
	movl var1(%rip), %edi
	movl var2(%rip), %esi
	call multiply
	movl %eax, -8(%rbp)
	# Print -8(%rbp)
	movl -8(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Less than: __temp2 = var1 < var2
	movl var1(%rip), %eax
	movl var2(%rip), %ebx
	cmpl %ebx, %eax
	jl lt_true_0
	movl $0, %eax
	movl %eax, -12(%rbp)
	jmp lt_end_1
lt_true_0:
	movl $1, %eax
	movl %eax, -12(%rbp)
lt_end_1:
	# Jump if zero to __label4
	movl -12(%rbp), %eax
	cmpl $0, %eax
	je __label4
	# Print "Subtraction, in modulo, of var1 and var2: "
	leaq str_5(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function subtract
	movl var2(%rip), %edi
	movl var1(%rip), %esi
	call subtract
	movl %eax, -16(%rbp)
	# Print -16(%rbp)
	movl -16(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Remainder of division of var2 by var1: "
	leaq str_6(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function remainder
	movl var2(%rip), %edi
	movl var1(%rip), %esi
	call remainder
	movl %eax, -20(%rbp)
	# Print -20(%rbp)
	movl -20(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Equal: __temp5 = var1 == $0
	movl var1(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	je eq_true_2
	movl $0, %eax
	movl %eax, -24(%rbp)
	jmp eq_end_3
eq_true_2:
	movl $1, %eax
	movl %eax, -24(%rbp)
eq_end_3:
	# Jump if zero to __label0
	movl -24(%rbp), %eax
	cmpl $0, %eax
	je __label0
	# Print "Division by zero is not allowed.\n"
	leaq str_7(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label1
__label0:
	# Print "Division of var2 by var1: "
	leaq str_8(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function divide
	movl var2(%rip), %edi
	movl var1(%rip), %esi
	call divide
	movl %eax, -28(%rbp)
	# Print -28(%rbp)
	movl -28(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label1:
	jmp __label5
__label4:
	# Print "Subtraction, in modulo, of var1 and var2: "
	leaq str_5(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function subtract
	movl var1(%rip), %edi
	movl var2(%rip), %esi
	call subtract
	movl %eax, -32(%rbp)
	# Print -32(%rbp)
	movl -32(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Remainder of division of var1 by var2: "
	leaq str_9(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function remainder
	movl var1(%rip), %edi
	movl var2(%rip), %esi
	call remainder
	movl %eax, -36(%rbp)
	# Print -36(%rbp)
	movl -36(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Equal: __temp9 = var2 == $0
	movl var2(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	je eq_true_4
	movl $0, %eax
	movl %eax, -40(%rbp)
	jmp eq_end_5
eq_true_4:
	movl $1, %eax
	movl %eax, -40(%rbp)
eq_end_5:
	# Jump if zero to __label2
	movl -40(%rbp), %eax
	cmpl $0, %eax
	je __label2
	# Print "Division by zero is not allowed.\n"
	leaq str_7(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label3
__label2:
	# Print "Division of var1 by var2: "
	leaq str_10(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Call function divide
	movl var1(%rip), %edi
	movl var2(%rip), %esi
	call divide
	movl %eax, -44(%rbp)
	# Print -44(%rbp)
	movl -44(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label3:
__label5:
	# Return $0
	movl $0, %eax
	# Function main epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

sum:
	# Function sum prologue
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	# Add: __temp11 = -4(%rbp) + -8(%rbp)
	movl -4(%rbp), %eax
	movl -8(%rbp), %ebx
	addl %ebx, %eax
	movl %eax, -12(%rbp)
	# Return -12(%rbp)
	movl -12(%rbp), %eax
	# Function sum epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

subtract:
	# Function subtract prologue
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	# Subtract: __temp12 = -4(%rbp) - -8(%rbp)
	movl -4(%rbp), %eax
	movl -8(%rbp), %ebx
	subl %ebx, %eax
	movl %eax, -12(%rbp)
	# Return -12(%rbp)
	movl -12(%rbp), %eax
	# Function subtract epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

multiply:
	# Function multiply prologue
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	# Multiply: __temp13 = -4(%rbp) * -8(%rbp)
	movl -4(%rbp), %eax
	movl -8(%rbp), %ebx
	imull %ebx, %eax
	movl %eax, -12(%rbp)
	# Return -12(%rbp)
	movl -12(%rbp), %eax
	# Function multiply epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

divide:
	# Function divide prologue
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	# Divide: __temp14 = -4(%rbp) / -8(%rbp)
	movl -4(%rbp), %eax
	cdq
	movl -8(%rbp), %ebx
	idivl %ebx
	movl %eax, -12(%rbp)
	# Return -12(%rbp)
	movl -12(%rbp), %eax
	# Function divide epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

remainder:
	# Function remainder prologue
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	# Modulo: __temp15 = -4(%rbp) % -8(%rbp)
	movl -4(%rbp), %eax
	cdq
	movl -8(%rbp), %ebx
	idivl %ebx
	movl %edx, -12(%rbp)
	# Return -12(%rbp)
	movl -12(%rbp), %eax
	# Function remainder epilogue
	movq %rbp, %rsp
	popq %rbp
	ret


.section .note.GNU-stack,"",@progbits
