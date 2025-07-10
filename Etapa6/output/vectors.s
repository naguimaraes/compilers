# Generated x86 Assembly Code from 2025++1 code
# Compiler made by Nathan Guimaraes (334437)

.section .data
	int_format: .string "%d"
	char_format: .string "%c"
	string_format: .string "%s"
	newline: .string "\n"
	scanf_int: .string "%d"
	scanf_char: .string " %c"
	i: .long 0
	sum: .long 0
	max: .long 0
	min: .long 0
	# Vector vector initialization
	vector:
		.long 1
		.long 2
		.long 3
		.long 4
		.long 5
		.long 6
		.long 7
		.long 8
		.long 9
		.long 10
	# Vector result initialization
	result:
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
	str_0: .string "Original vector: "
	str_1: .string " "
	str_2: .string "\n"
	str_3: .string "Calculating sum...\n"
	str_4: .string "Sum: "
	str_5: .string "Finding max and min...\n"
	str_6: .string "Max: "
	str_7: .string ", Min: "
	str_8: .string "Creating doubled vector...\n"
	str_9: .string "Doubled vector: "
	str_10: .string "Reverse order: "

.section .text
.globl main
.extern printf
.extern scanf

main:
	# Function main prologue
	pushq %rbp
	movq %rsp, %rbp
	# Print "Original vector: "
	leaq str_0(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label0:
	# Less than: __temp0 = i < $10
	movl i(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_0
	movl $0, %eax
	movl %eax, -4(%rbp)
	jmp lt_end_1
lt_true_0:
	movl $1, %eax
	movl %eax, -4(%rbp)
lt_end_1:
	# Jump if zero to __label1
	movl -4(%rbp), %eax
	cmpl $0, %eax
	je __label1
	# Vector read: __temp1 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -8(%rbp)
	# Print -8(%rbp)
	movl -8(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print " "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Add: __temp2 = i + $1
	movl i(%rip), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -12(%rbp)
	# Move -12(%rbp) to i
	movl -12(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label0
__label1:
	# Print "\n"
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Calculating sum...\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move $0 to i
	movl $0, i(%rip)
	# Move $0 to sum
	movl $0, sum(%rip)
__label2:
	# Less than: __temp3 = i < $10
	movl i(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_2
	movl $0, %eax
	movl %eax, -16(%rbp)
	jmp lt_end_3
lt_true_2:
	movl $1, %eax
	movl %eax, -16(%rbp)
lt_end_3:
	# Jump if zero to __label3
	movl -16(%rbp), %eax
	cmpl $0, %eax
	je __label3
	# Vector read: __temp4 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -20(%rbp)
	# Add: __temp5 = sum + -20(%rbp)
	movl sum(%rip), %eax
	movl -20(%rbp), %ebx
	addl %ebx, %eax
	movl %eax, -24(%rbp)
	# Move -24(%rbp) to sum
	movl -24(%rbp), %eax
	movl %eax, sum(%rip)
	# Add: __temp6 = i + $1
	movl i(%rip), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -28(%rbp)
	# Move -28(%rbp) to i
	movl -28(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label2
__label3:
	# Print "Sum: "
	leaq str_4(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print sum
	movl sum(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Finding max and min...\n"
	leaq str_5(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move $0 to i
	movl $0, i(%rip)
	# Vector read: __temp7 = vector[$0]
	movl $0, %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -32(%rbp)
	# Move -32(%rbp) to max
	movl -32(%rbp), %eax
	movl %eax, max(%rip)
	# Vector read: __temp8 = vector[$0]
	movl $0, %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -36(%rbp)
	# Move -36(%rbp) to min
	movl -36(%rbp), %eax
	movl %eax, min(%rip)
__label4:
	# Less than: __temp9 = i < $10
	movl i(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_4
	movl $0, %eax
	movl %eax, -40(%rbp)
	jmp lt_end_5
lt_true_4:
	movl $1, %eax
	movl %eax, -40(%rbp)
lt_end_5:
	# Jump if zero to __label5
	movl -40(%rbp), %eax
	cmpl $0, %eax
	je __label5
	# Vector read: __temp10 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -44(%rbp)
	# Greater than: __temp11 = -44(%rbp) > max
	movl -44(%rbp), %eax
	movl max(%rip), %ebx
	cmpl %ebx, %eax
	jg gt_true_6
	movl $0, %eax
	movl %eax, -48(%rbp)
	jmp gt_end_7
gt_true_6:
	movl $1, %eax
	movl %eax, -48(%rbp)
gt_end_7:
	# Jump if zero to __label6
	movl -48(%rbp), %eax
	cmpl $0, %eax
	je __label6
	# Vector read: __temp12 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -52(%rbp)
	# Move -52(%rbp) to max
	movl -52(%rbp), %eax
	movl %eax, max(%rip)
__label6:
	# Vector read: __temp13 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -56(%rbp)
	# Less than: __temp14 = -56(%rbp) < min
	movl -56(%rbp), %eax
	movl min(%rip), %ebx
	cmpl %ebx, %eax
	jl lt_true_8
	movl $0, %eax
	movl %eax, -60(%rbp)
	jmp lt_end_9
lt_true_8:
	movl $1, %eax
	movl %eax, -60(%rbp)
lt_end_9:
	# Jump if zero to __label8
	movl -60(%rbp), %eax
	cmpl $0, %eax
	je __label8
	# Vector read: __temp15 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -64(%rbp)
	# Move -64(%rbp) to min
	movl -64(%rbp), %eax
	movl %eax, min(%rip)
__label8:
	# Add: __temp16 = i + $1
	movl i(%rip), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -68(%rbp)
	# Move -68(%rbp) to i
	movl -68(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label4
__label5:
	# Print "Max: "
	leaq str_6(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print max
	movl max(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print ", Min: "
	leaq str_7(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print min
	movl min(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Creating doubled vector...\n"
	leaq str_8(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move $0 to i
	movl $0, i(%rip)
__label10:
	# Less than: __temp17 = i < $10
	movl i(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_10
	movl $0, %eax
	movl %eax, -72(%rbp)
	jmp lt_end_11
lt_true_10:
	movl $1, %eax
	movl %eax, -72(%rbp)
lt_end_11:
	# Jump if zero to __label11
	movl -72(%rbp), %eax
	cmpl $0, %eax
	je __label11
	# Vector read: __temp18 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -76(%rbp)
	# Multiply: __temp19 = -76(%rbp) * $2
	movl -76(%rbp), %eax
	movl $2, %ebx
	imull %ebx, %eax
	movl %eax, -80(%rbp)
	# Vector write: result[i] = -80(%rbp)
	movl i(%rip), %eax
	imull $4, %eax
	movl -80(%rbp), %ebx
	leaq result(%rip), %rcx
	movl %ebx, (%rcx,%rax)
	# Add: __temp20 = i + $1
	movl i(%rip), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -84(%rbp)
	# Move -84(%rbp) to i
	movl -84(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label10
__label11:
	# Print "Doubled vector: "
	leaq str_9(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move $0 to i
	movl $0, i(%rip)
__label12:
	# Less than: __temp21 = i < $10
	movl i(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_12
	movl $0, %eax
	movl %eax, -88(%rbp)
	jmp lt_end_13
lt_true_12:
	movl $1, %eax
	movl %eax, -88(%rbp)
lt_end_13:
	# Jump if zero to __label13
	movl -88(%rbp), %eax
	cmpl $0, %eax
	je __label13
	# Vector read: __temp22 = result[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq result(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -92(%rbp)
	# Print -92(%rbp)
	movl -92(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print " "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Add: __temp23 = i + $1
	movl i(%rip), %eax
	movl $1, %ebx
	addl %ebx, %eax
	movl %eax, -96(%rbp)
	# Move -96(%rbp) to i
	movl -96(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label12
__label13:
	# Print "\n"
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print "Reverse order: "
	leaq str_10(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move $9 to i
	movl $9, i(%rip)
__label14:
	# Greater or Equal: __temp24 = i >= $0
	movl i(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jge ge_true_14
	movl $0, %eax
	movl %eax, -100(%rbp)
	jmp ge_end_15
ge_true_14:
	movl $1, %eax
	movl %eax, -100(%rbp)
ge_end_15:
	# Jump if zero to __label15
	movl -100(%rbp), %eax
	cmpl $0, %eax
	je __label15
	# Vector read: __temp25 = vector[i]
	movl i(%rip), %eax
	imull $4, %eax
	leaq vector(%rip), %rcx
	movl (%rcx,%rax), %ebx
	movl %ebx, -104(%rbp)
	# Print -104(%rbp)
	movl -104(%rbp), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print " "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Subtract: __temp26 = i - $1
	movl i(%rip), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, -108(%rbp)
	# Move -108(%rbp) to i
	movl -108(%rbp), %eax
	movl %eax, i(%rip)
	jmp __label14
__label15:
	# Print "\n"
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Return $0
	movl $0, %eax
	# Function main epilogue
	movq %rbp, %rsp
	popq %rbp
	ret


.section .note.GNU-stack,"",@progbits
