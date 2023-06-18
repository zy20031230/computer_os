STACK_SIZE =	0x1000	# Size of heap and stack. 4KB

/* ===== code32 =========================== */

	.globl 	_start		# GNU default entry point
	.globl 	osStart
.text
.code32
_start:
	jmp establish_stack

dead:	jmp dead			# Never here

# Set up the stack
establish_stack:
	; movl	_end, %eax		# eax = end of bss/start of heap #填入正确的内容
	; addl	$STACK_SIZE, %eax	# make room for stack
	; andl	$0xffffffc0, %eax	# align it on 16 byte boundary
	movl	$0x80000, %eax	
	
	movl	%eax, %esp		# set stack pointer
	movl	%eax, %ebp		# set base pointer

# Zero out the BSS segment
zero_bss:
	cld				              # make direction flag count up
	movl	$_end, %ecx		    # find end of .bss
	movl	$_bss_start, %edi	# edi = beginning of .bss
	subl	%edi, %ecx		    # ecx = size of .bss in bytes
	shrl	%ecx			        # size of .bss in longs
	shrl	%ecx
	
	xorl	%eax, %eax		    # value to clear out memory
	repne			            	# while ecx != 0
	stosl				            # clear a long in the bss

	
setup_idt:
	movl $ignore_int1,%edx # ignore应该是一个32位的地址
	movl $0x00080000,%eax # edx应该是作为高位的数据,地址在向上生长.
	movw %dx,%ax /* selector = 0x0010 = cs */
	movw $0x8E00,%dx /* interrupt gate - dpl=0, present */
	movl $IDT,%edi
	mov $256,%ecx
	
rp_sidt:
	movl %eax,(%edi) //代表了中断向量表的位置,eax应该指的是时钟调用的中断在向量表的第一条
	movl %edx,4(%edi) //这个代表了,没有这个服务,会去调用no such 指令
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	// 应该是,中断向量表每一条指令有64位,上述操作每次存储了一条指令.然后上完成了终端向量表全
	// ignore_body的初始化.

	lidt idtptr
	
	call setup_time_int_32
	
# Transfer control to main
to_main:	
	call	osStart

shut_down:	
	jmp	shut_down	# Never here	

	.p2align 4
time_interrupt:
	cld
	pushf
	pusha
	call tick # 保存了寄存器标志位的相关状态,然后调用了tick
	popa
	popf
	iret
//你需要填写它
	
	.p2align 4
ignore_int1:
//你需要填写它
	cld
	pusha
	call ignoreIntBody
	popa
	iret
	

# ret /* if do not set timer*/
setup_time_int_32:
	movl $time_interrupt,%edx
	movl $0x00080000,%eax /* selector：0x0010 = cs */
	movw %dx,%ax
	movw $0x8E00,%dx /* interrupt gate - dpl=0, present */
	movl $IDT,%edi
	addl $(32*8), %edi
	movl %eax,(%edi)
	movl %edx,4(%edi)
	ret


/* ===== data =========================== */
.data
# IDT
	.p2align 4
	.globl IDT
IDT:

	.rept 256
	.word 0,0,0,0
	.endr
idtptr:

	.word (256*8 - 1)
	.long IDT
