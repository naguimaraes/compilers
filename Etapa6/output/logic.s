# Generated x86 Assembly Code from 2025++1 code
# Compiler made by Nathan Guimaraes (334437)

.section .data
	int_format: .string "%d"
	char_format: .string "%c"
	string_format: .string "%s"
	newline: .string "\n"
	scanf_int: .string "%d"
	scanf_char: .string " %c"
	x: .long 0
	y: .long 0
	counter: .long 0
	counterBuffer: .long 0
	str_0: .string "Enter a value for x: "
	str_1: .string "Enter a value for y: "
	str_2: .string "Enter a value for counter: "
	str_3: .string "\nComparison Tests:\n"
	str_4: .string "x > y: TRUE\n"
	str_5: .string "x > y: FALSE\n"
	str_6: .string "x < y: TRUE\n"
	str_7: .string "x < y: FALSE\n"
	str_8: .string "x >= y: TRUE\n"
	str_9: .string "x >= y: FALSE\n"
	str_10: .string "x <= y: TRUE\n"
	str_11: .string "x <= y: FALSE\n"
	str_12: .string "x == y: TRUE\n"
	str_13: .string "x == y: FALSE\n"
	str_14: .string "x != y: TRUE\n"
	str_15: .string "x != y: FALSE\n"
	str_16: .string "\nLogical Tests:\n"
	str_17: .string "Both positive: TRUE\n"
	str_18: .string "Both positive: FALSE\n"
	str_19: .string "At least one positive: TRUE\n"
	str_20: .string "At least one positive: FALSE\n"
	str_21: .string "Both negative: TRUE\n"
	str_22: .string "Both negative: FALSE\n"
	str_23: .string "\nMOD Tests:\n"
	str_24: .string "x is even\n"
	str_25: .string "x is odd\n"
	str_26: .string "y divisible by 3\n"
	str_27: .string "y not divisible by 3\n"
	str_28: .string "\nLoop tests should repeat "
	str_29: .string " times.\n"
	str_30: .string "While-Do Test:\n"
	str_31: .string "Counter: "
	str_32: .string "\n"
	str_33: .string "\nDo-While Test:\n"
	str_34: .string "\nNested Conditions:\n"
	str_35: .string "x > 5 AND y < 10\n"
	str_36: .string "x > 5 BUT y >= 10\n"
	str_37: .string "x <= 5 BUT y < 10\n"
	str_38: .string "x <= 5 AND y >= 10\n"
	str_39: .string "\nIf Without Else Test:\n"
	str_40: .string "x is positive\n"
	str_41: .string "y is zero\n"

.section .text
.globl main
.extern printf
.extern scanf

main:
	# Function main prologue
	pushq %rbp
	movq %rsp, %rbp
	# Print "Enter a value for x: "
	leaq str_0(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into x
	leaq x(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "Enter a value for y: "
	leaq str_1(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into y
	leaq y(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "Enter a value for counter: "
	leaq str_2(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Read into counter
	leaq counter(%rip), %rsi
	leaq scanf_int(%rip), %rdi
	call scanf@PLT
	# Print "\nComparison Tests:\n"
	leaq str_3(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Greater than: __temp0 = x > y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	jg gt_true_0
	movl $0, %eax
	movl %eax, -4(%rbp)
	jmp gt_end_1
gt_true_0:
	movl $1, %eax
	movl %eax, -4(%rbp)
gt_end_1:
	# Jump if zero to __label0
	movl -4(%rbp), %eax
	cmpl $0, %eax
	je __label0
	# Print "x > y: TRUE\n"
	leaq str_4(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label1
__label0:
	# Print "x > y: FALSE\n"
	leaq str_5(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label1:
	# Less than: __temp1 = x < y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	jl lt_true_2
	movl $0, %eax
	movl %eax, -8(%rbp)
	jmp lt_end_3
lt_true_2:
	movl $1, %eax
	movl %eax, -8(%rbp)
lt_end_3:
	# Jump if zero to __label2
	movl -8(%rbp), %eax
	cmpl $0, %eax
	je __label2
	# Print "x < y: TRUE\n"
	leaq str_6(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label3
__label2:
	# Print "x < y: FALSE\n"
	leaq str_7(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label3:
	# Greater or Equal: __temp2 = x >= y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	jge ge_true_4
	movl $0, %eax
	movl %eax, -12(%rbp)
	jmp ge_end_5
ge_true_4:
	movl $1, %eax
	movl %eax, -12(%rbp)
ge_end_5:
	# Jump if zero to __label4
	movl -12(%rbp), %eax
	cmpl $0, %eax
	je __label4
	# Print "x >= y: TRUE\n"
	leaq str_8(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label5
__label4:
	# Print "x >= y: FALSE\n"
	leaq str_9(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label5:
	# Less or Equal: __temp3 = x <= y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	jle le_true_6
	movl $0, %eax
	movl %eax, -16(%rbp)
	jmp le_end_7
le_true_6:
	movl $1, %eax
	movl %eax, -16(%rbp)
le_end_7:
	# Jump if zero to __label6
	movl -16(%rbp), %eax
	cmpl $0, %eax
	je __label6
	# Print "x <= y: TRUE\n"
	leaq str_10(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label7
__label6:
	# Print "x <= y: FALSE\n"
	leaq str_11(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label7:
	# Equal: __temp4 = x == y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	je eq_true_8
	movl $0, %eax
	movl %eax, -20(%rbp)
	jmp eq_end_9
eq_true_8:
	movl $1, %eax
	movl %eax, -20(%rbp)
eq_end_9:
	# Jump if zero to __label8
	movl -20(%rbp), %eax
	cmpl $0, %eax
	je __label8
	# Print "x == y: TRUE\n"
	leaq str_12(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label9
__label8:
	# Print "x == y: FALSE\n"
	leaq str_13(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label9:
	# Not Equal: __temp5 = x != y
	movl x(%rip), %eax
	movl y(%rip), %ebx
	cmpl %ebx, %eax
	jne ne_true_10
	movl $0, %eax
	movl %eax, -24(%rbp)
	jmp ne_end_11
ne_true_10:
	movl $1, %eax
	movl %eax, -24(%rbp)
ne_end_11:
	# Jump if zero to __label10
	movl -24(%rbp), %eax
	cmpl $0, %eax
	je __label10
	# Print "x != y: TRUE\n"
	leaq str_14(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label11
__label10:
	# Print "x != y: FALSE\n"
	leaq str_15(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label11:
	# Print "\nLogical Tests:\n"
	leaq str_16(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Greater than: __temp6 = x > $0
	movl x(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_12
	movl $0, %eax
	movl %eax, -28(%rbp)
	jmp gt_end_13
gt_true_12:
	movl $1, %eax
	movl %eax, -28(%rbp)
gt_end_13:
	# Greater than: __temp7 = y > $0
	movl y(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_14
	movl $0, %eax
	movl %eax, -32(%rbp)
	jmp gt_end_15
gt_true_14:
	movl $1, %eax
	movl %eax, -32(%rbp)
gt_end_15:
	# Logical AND: __temp8 = -28(%rbp) && -32(%rbp)
	movl -28(%rbp), %eax
	cmpl $0, %eax
	je and_false_16
	movl -32(%rbp), %eax
	cmpl $0, %eax
	je and_false_16
	movl $1, %eax
	movl %eax, -36(%rbp)
	jmp and_end_17
and_false_16:
	movl $0, %eax
	movl %eax, -36(%rbp)
and_end_17:
	# Jump if zero to __label12
	movl -36(%rbp), %eax
	cmpl $0, %eax
	je __label12
	# Print "Both positive: TRUE\n"
	leaq str_17(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label13
__label12:
	# Print "Both positive: FALSE\n"
	leaq str_18(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label13:
	# Greater than: __temp9 = x > $0
	movl x(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_18
	movl $0, %eax
	movl %eax, -40(%rbp)
	jmp gt_end_19
gt_true_18:
	movl $1, %eax
	movl %eax, -40(%rbp)
gt_end_19:
	# Greater than: __temp10 = y > $0
	movl y(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_20
	movl $0, %eax
	movl %eax, -44(%rbp)
	jmp gt_end_21
gt_true_20:
	movl $1, %eax
	movl %eax, -44(%rbp)
gt_end_21:
	# Logical OR: __temp11 = -40(%rbp) || -44(%rbp)
	movl -40(%rbp), %eax
	cmpl $0, %eax
	jne or_true_22
	movl -44(%rbp), %eax
	cmpl $0, %eax
	jne or_true_22
	movl $0, %eax
	movl %eax, -48(%rbp)
	jmp or_end_23
or_true_22:
	movl $1, %eax
	movl %eax, -48(%rbp)
or_end_23:
	# Jump if zero to __label14
	movl -48(%rbp), %eax
	cmpl $0, %eax
	je __label14
	# Print "At least one positive: TRUE\n"
	leaq str_19(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label15
__label14:
	# Print "At least one positive: FALSE\n"
	leaq str_20(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label15:
	# Less than: __temp12 = x < $0
	movl x(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jl lt_true_24
	movl $0, %eax
	movl %eax, -52(%rbp)
	jmp lt_end_25
lt_true_24:
	movl $1, %eax
	movl %eax, -52(%rbp)
lt_end_25:
	# Less than: __temp13 = y < $0
	movl y(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jl lt_true_26
	movl $0, %eax
	movl %eax, -56(%rbp)
	jmp lt_end_27
lt_true_26:
	movl $1, %eax
	movl %eax, -56(%rbp)
lt_end_27:
	# Logical AND: __temp14 = -52(%rbp) && -56(%rbp)
	movl -52(%rbp), %eax
	cmpl $0, %eax
	je and_false_28
	movl -56(%rbp), %eax
	cmpl $0, %eax
	je and_false_28
	movl $1, %eax
	movl %eax, -60(%rbp)
	jmp and_end_29
and_false_28:
	movl $0, %eax
	movl %eax, -60(%rbp)
and_end_29:
	# Jump if zero to __label16
	movl -60(%rbp), %eax
	cmpl $0, %eax
	je __label16
	# Print "Both negative: TRUE\n"
	leaq str_21(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label17
__label16:
	# Print "Both negative: FALSE\n"
	leaq str_22(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label17:
	# Print "\nMOD Tests:\n"
	leaq str_23(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Modulo: __temp15 = x % $2
	movl x(%rip), %eax
	cdq
	movl $2, %ebx
	idivl %ebx
	movl %edx, -64(%rbp)
	# Equal: __temp16 = -64(%rbp) == $0
	movl -64(%rbp), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	je eq_true_30
	movl $0, %eax
	movl %eax, -68(%rbp)
	jmp eq_end_31
eq_true_30:
	movl $1, %eax
	movl %eax, -68(%rbp)
eq_end_31:
	# Jump if zero to __label18
	movl -68(%rbp), %eax
	cmpl $0, %eax
	je __label18
	# Print "x is even\n"
	leaq str_24(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label19
__label18:
	# Print "x is odd\n"
	leaq str_25(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label19:
	# Modulo: __temp17 = y % $3
	movl y(%rip), %eax
	cdq
	movl $3, %ebx
	idivl %ebx
	movl %edx, -72(%rbp)
	# Equal: __temp18 = -72(%rbp) == $0
	movl -72(%rbp), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	je eq_true_32
	movl $0, %eax
	movl %eax, -76(%rbp)
	jmp eq_end_33
eq_true_32:
	movl $1, %eax
	movl %eax, -76(%rbp)
eq_end_33:
	# Jump if zero to __label20
	movl -76(%rbp), %eax
	cmpl $0, %eax
	je __label20
	# Print "y divisible by 3\n"
	leaq str_26(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label21
__label20:
	# Print "y not divisible by 3\n"
	leaq str_27(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label21:
	# Print "\nLoop tests should repeat "
	leaq str_28(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print counter
	movl counter(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print " times.\n"
	leaq str_29(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move counter to counterBuffer
	movl counter(%rip), %eax
	movl %eax, counterBuffer(%rip)
	# Print "While-Do Test:\n"
	leaq str_30(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label22:
	# Less than: __temp19 = $0 < counter
	movl $0, %eax
	movl counter(%rip), %ebx
	cmpl %ebx, %eax
	jl lt_true_34
	movl $0, %eax
	movl %eax, -80(%rbp)
	jmp lt_end_35
lt_true_34:
	movl $1, %eax
	movl %eax, -80(%rbp)
lt_end_35:
	# Jump if zero to __label23
	movl -80(%rbp), %eax
	cmpl $0, %eax
	je __label23
	# Print "Counter: "
	leaq str_31(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print counter
	movl counter(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_32(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Subtract: __temp20 = counter - $1
	movl counter(%rip), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, -84(%rbp)
	# Move -84(%rbp) to counter
	movl -84(%rbp), %eax
	movl %eax, counter(%rip)
	jmp __label22
__label23:
	# Print "\nDo-While Test:\n"
	leaq str_33(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Move counterBuffer to counter
	movl counterBuffer(%rip), %eax
	movl %eax, counter(%rip)
__label24:
	# Print "Counter: "
	leaq str_31(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Print counter
	movl counter(%rip), %esi
	leaq int_format(%rip), %rdi
	call printf@PLT
	# Print "\n"
	leaq str_32(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Subtract: __temp21 = counter - $1
	movl counter(%rip), %eax
	movl $1, %ebx
	subl %ebx, %eax
	movl %eax, -88(%rbp)
	# Move -88(%rbp) to counter
	movl -88(%rbp), %eax
	movl %eax, counter(%rip)
	# Greater than: __temp22 = counter > $0
	movl counter(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_36
	movl $0, %eax
	movl %eax, -92(%rbp)
	jmp gt_end_37
gt_true_36:
	movl $1, %eax
	movl %eax, -92(%rbp)
gt_end_37:
	# Jump if zero to __label25
	movl -92(%rbp), %eax
	cmpl $0, %eax
	je __label25
	jmp __label24
__label25:
	# Print "\nNested Conditions:\n"
	leaq str_34(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Greater than: __temp23 = x > $5
	movl x(%rip), %eax
	movl $5, %ebx
	cmpl %ebx, %eax
	jg gt_true_38
	movl $0, %eax
	movl %eax, -96(%rbp)
	jmp gt_end_39
gt_true_38:
	movl $1, %eax
	movl %eax, -96(%rbp)
gt_end_39:
	# Jump if zero to __label30
	movl -96(%rbp), %eax
	cmpl $0, %eax
	je __label30
	# Less than: __temp24 = y < $10
	movl y(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_40
	movl $0, %eax
	movl %eax, -100(%rbp)
	jmp lt_end_41
lt_true_40:
	movl $1, %eax
	movl %eax, -100(%rbp)
lt_end_41:
	# Jump if zero to __label26
	movl -100(%rbp), %eax
	cmpl $0, %eax
	je __label26
	# Print "x > 5 AND y < 10\n"
	leaq str_35(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label27
__label26:
	# Print "x > 5 BUT y >= 10\n"
	leaq str_36(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label27:
	jmp __label31
__label30:
	# Less than: __temp25 = y < $10
	movl y(%rip), %eax
	movl $10, %ebx
	cmpl %ebx, %eax
	jl lt_true_42
	movl $0, %eax
	movl %eax, -104(%rbp)
	jmp lt_end_43
lt_true_42:
	movl $1, %eax
	movl %eax, -104(%rbp)
lt_end_43:
	# Jump if zero to __label28
	movl -104(%rbp), %eax
	cmpl $0, %eax
	je __label28
	# Print "x <= 5 BUT y < 10\n"
	leaq str_37(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	jmp __label29
__label28:
	# Print "x <= 5 AND y >= 10\n"
	leaq str_38(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label29:
__label31:
	# Print "\nIf Without Else Test:\n"
	leaq str_39(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
	# Greater than: __temp26 = x > $0
	movl x(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	jg gt_true_44
	movl $0, %eax
	movl %eax, -108(%rbp)
	jmp gt_end_45
gt_true_44:
	movl $1, %eax
	movl %eax, -108(%rbp)
gt_end_45:
	# Jump if zero to __label32
	movl -108(%rbp), %eax
	cmpl $0, %eax
	je __label32
	# Print "x is positive\n"
	leaq str_40(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label32:
	# Equal: __temp27 = y == $0
	movl y(%rip), %eax
	movl $0, %ebx
	cmpl %ebx, %eax
	je eq_true_46
	movl $0, %eax
	movl %eax, -112(%rbp)
	jmp eq_end_47
eq_true_46:
	movl $1, %eax
	movl %eax, -112(%rbp)
eq_end_47:
	# Jump if zero to __label34
	movl -112(%rbp), %eax
	cmpl $0, %eax
	je __label34
	# Print "y is zero\n"
	leaq str_41(%rip), %rsi
	leaq string_format(%rip), %rdi
	call printf@PLT
__label34:
	# Return $0
	movl $0, %eax
	# Function main epilogue
	movq %rbp, %rsp
	popq %rbp
	ret


.section .note.GNU-stack,"",@progbits
