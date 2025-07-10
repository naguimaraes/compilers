# Generated x86 Assembly Code from 2025++1 code
# Compiler made by Nathan Guimaraes (334437)

.section .data
	int_format: .string "%d"
	char_format: .string "%c"
	string_format: .string "%s"
	newline: .string "\n"
	scanf_int: .string "%d"
	scanf_char: .string " %c"
	readVarInt: .long 0
	charTest: .long 97
	str_0: .string "Entering an infinite loop to read integers, please enter 4 to exit: \n"
	str_1: .string "Enter an integer: "
	str_2: .string "Read value: "
	str_3: .string "\n"
	str_4: .string "Exited the loop.\n"

.section .text
.globl main
.extern printf
.extern scanf

main:
	# Function main prologue
	pushq %rbp
	movq %rsp, %rbp
	# Print "Entering an infinite loop to read integers, please enter 4 to exit: \n"
	leaq str_0(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label0:
	# Not Equal: __temp0 = readVarInt != $4
	movl readVarInt(%rip), %eax
	movl $4, %ebx
	cmpl %ebx, %eax
	jne ne_true_0
	movl $0, %eax
	movl %eax, -4(%rbp)
	jmp ne_end_1
ne_true_0:
	movl $1, %eax
	movl %eax, -4(%rbp)
ne_end_1:
	# Jump if zero to __label1
	movl -4(%rbp), %eax
	cmpl $0, %eax
	je __label1
	# Print "Enter an integer: "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into readVarInt
	leaq readVarInt(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "Read value: "
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print readVarInt
	movl readVarInt(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label0
__label1:
	# Print "Exited the loop.\n"
	leaq str_4(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Return $0
	movl $0, %eax
	# Function main epilogue
	movq %rbp, %rsp
	popq %rbp
	ret


.section .note.GNU-stack,"",@progbits
