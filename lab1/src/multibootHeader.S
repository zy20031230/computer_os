.globl start       #一般都用start

magic = 0x1BADB002
flags = 0x00000000
checksum = -(magic + flags)

/*此处，按照multiboot协议要求定义必要的参数*/
/*格式：XXX_ITEM_NAME=value*/
/*value可以是十六进制的（0x`开头）、十进制的等等*/
/*为便于阅读，可以适当增加一些注释和空行*/
/*此处开始，按协议标准来定义必须的multiboot header*/
.section “.multiboot_header”      #先起一个section的名字
.align 4
.long magic
.long flags
.long checksum
/*使用.long和前面定义的参数构建头结构，每次32位，格式为：.long XXX_ITEM_NAME*/
.text  #进入代码段
.code32  #32位代码
start:          # 这个跟第一行的声明要一致
    movl $0, %eax # 由于显示出了大量无效字符,所以显示清零
    movl $0xB8000, %ebx # 给予显存地址
clean:
    movl $0, (%ebx,%eax,4) # 每一次操作四个字节
    addl $1, %eax 
    cmpl $0x03e8, %eax # 查询资料知一屏幕内容有4000个字节,选择大于4000字节的内存空间完成清理
    jl clean
/*下面屏幕输出OK或其他字符序列*/
/*使用指令movl $0x12345678, 0xB8000*/     #0x1234和0x5678各自输出1个字符
/*根据需要输出多个字符，也可以使用其他mov指令*/
vgaoutput:
	movl $0x2f652f48, 0xB8000 # movl指令,前面的地址放在后面,也就是地址从高到低进行分布
	movl $0x2f6C2f6C, 0xB8004
	movl $0x2f202f6F, 0xB8008
	movl $0x2f6F2f77, 0xB800C
	movl $0x2f6C2f72, 0xB8010
	movl $0x2f212f64, 0xB8014
	movl $0x2f502f20, 0xB8018
	movl $0x2f322f42, 0xB801C
	movl $0x2f302f31, 0xB8020
	movl $0x2f302f32, 0xB8024
	movl $0x2f352f35, 0xB8028
	movl $0x2f5f2f32, 0xB802C
	movl $0x2f682f7a, 0xB8030
	movl $0x2f6e2f61, 0xB8034
	movl $0x2f792f67, 0xB8038
	movl $0x00002f69, 0xB803c
/*可以根据需要使用nop指令隔开不同功能片段，也可以适当使用空行*/
/*根据需要初始化串口*/
/*根据需要串口输出你的字符序列，详见前面串口编程简介*/
Uart_output:   # 一下是利用串口输出helloworld的内容
	movw $0x3f8, %dx
	nop
	movb $0x48, %al
	outb %al, %dx
	movb $0x45, %al
	outb %al, %dx
	movb $0x4c, %al
	outb %al, %dx
	movb $0x4c, %al
	outb %al, %dx
	movb $0x4f, %al
	outb %al, %dx
	movb $0x57, %al
	outb %al, %dx
	movb $0x4f, %al
	outb %al, %dx
	movb $0x52, %al
	outb %al, %dx
	movb $0x4c, %al
	outb %al, %dx
	movb $0x44, %al
	outb %al, %dx
	movb $0x5f, %al
	outb %al, %dx
	movb $0x7a, %al
	outb %al, %dx
	movb $0x68, %al
	outb %al, %dx
	movb $0x61, %al
	outb %al, %dx
	movb $0x6E, %al
	outb %al, %dx
	movb $0x67, %al
	outb %al, %dx
	movb $0x79, %al
	outb %al, %dx
	movb $0x69, %al
	outb %al, %dx

hlt /*实验结束，让计算机停机，方法：使用hlt指令，或者死循环*/

