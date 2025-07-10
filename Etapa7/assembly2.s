# Generated x86 Assembly Code from 2025++1 code
# Compiler made by Nathan Guimaraes (334437)

.section .data
	int_format: .string "%d"
	char_format: .string "%c"
	string_format: .string "%s"
	newline: .string "\n"
	scanf_int: .string "%d"
	scanf_char: .string " %c"
	str_0: .string "y é maior que 5"

.section .text
.globl main
.extern printf
.extern scanf

main:
	# Function main prologue
	pushq %rbp
	movq %rsp, %rbp
	# Greater than: __temp1 = -4(%rbp) > $5
	movl -4(%rbp), %eax
	movl $5, %ebx
	cmpl %ebx, %eax
	jg gt_true_0
	movl $0, %eax
	movl %eax, -8(%rbp)
	jmp gt_end_1
gt_true_0:
	movl $1, %eax
	movl %eax, -8(%rbp)
gt_end_1:
	# Jump if zero to __label0
	movl -8(%rbp), %eax
	cmpl $0, %eax
	je __label0
	# Print "y é maior que 5"
	leaq str_0(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label0:
	# Add: __temp2 = -12(%rbp) + $5
	movl -12(%rbp), %eax
	movl $5, %ebx
	addl %ebx, %eax
	movl %eax, -16(%rbp)
	# Move -16(%rbp) to y
	movl -16(%rbp), %eax
	movl %eax, -4(%rbp)
	# Return $0
	movl $0, %eax
	# Function main epilogue
	movq %rbp, %rsp
	popq %rbp
	ret

func2:
	# Function func2 prologue
	pushq %rbp
	movq %rsp, %rbp
	# Add: __temp0 = -12(%rbp) + -4(%rbp)
	movl -12(%rbp), %eax
	movl -4(%rbp), %ebx
	addl %ebx, %eax
	movl %eax, -4(%rbp)
	# Return -4(%rbp)
	movl -4(%rbp), %eax
	# Function func2 epilogue
	movq %rbp, %rsp
	popq %rbp
	ret


.section .note.GNU-stack,"",@progbits
