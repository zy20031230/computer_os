#include "io.h"

void init8253(void){
	//你需要填写它
	unsigned int fre_div=1193180/100;//定义需要完成计数的值
	outb(0x43,0x34);//高低字节均需要完成相关操作
	unsigned char low,high;
	low = fre_div;
	high = fre_div >>8;
	outb(0x40,low);
	outb(0x40,high);
	outb(0x21,inb(0x21)&0xFE);//如果完成了计算,发出中断.
}
