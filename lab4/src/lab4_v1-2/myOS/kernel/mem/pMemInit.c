#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;  // 可用的内存的起始地址
unsigned long pMemSize;  // 可用的大小

unsigned long kMemStart;
unsigned long uMemStart;

void memTest(unsigned long start, unsigned long grainSize){
	// TODO
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从start开始有多大的内存可用，具体算法参照ppt检测算法的文字描述
	注意点三个：
	1、覆盖写入和读出就是往指针指向的位置写和读，不要想复杂。
	  (至于为什么这种检测内存的方法可行大家要自己想一下)
	2、开始的地址要大于1M，需要做一个if判断。
	3、grainsize不能太小，也要做一个if判断
	
	*/
// 检测算法可以知道 grainSize至少需要两个字节去完成写入的检查,同时开始的地址需要大于1m
	unsigned short *input_head,*input_tail;
	unsigned long  addr = start;
	unsigned short  keep;//用来复写原本的内容
	unsigned short check_1 = 0xAA55;
	unsigned short check_2 = 0x55AA;
	int flag = 1;
	if(start < 0x100000) start = 0x100000;//1m
	pMemStart = start;
	if(grainSize < 2) grainSize = 2;// limit 2
	while(flag){
		input_head = (unsigned short *)addr;
		input_tail = (unsigned short *)(addr + grainSize -2);
		keep = *input_head;
		*input_head = check_1;
		if(*input_head != check_1) flag = 0;
		*input_head =check_2;
		if(*input_head != check_2) flag= 0;
		*input_head = keep;
		//以上完成所有的首字节检查
		keep = *input_tail;
		*input_tail = check_1;
		if(*input_tail != check_1)flag = 0;
		*input_tail = check_2;
		if(*input_tail != check_2) flag =0;
		*input_tail = keep;
		//尾字节检测
		addr += grainSize;
	}
	//以上字节向前越位一个
	addr -= grainSize;
	pMemSize=addr-pMemStart;
	myPrintk(0x7,"MemStart: %x  \n", pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n", pMemSize);
	// myPrintk(0x7,"MemSize:  %x  \n", addr - pMemStart);

	
}
extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	myPrintk(0x7,"%x",pMemSize);
	// 此处选择不同的内存管理算法
	pMemHandler = dPartitionInit(pMemStart,pMemSize);

	//分配start
    kMemStart = dPartitionAlloc(pMemHandler, pMemSize / 3);
    uMemStart = dPartitionAlloc(pMemHandler, pMemSize / 3);
	//初始化句柄

	kMemHandler = dPartitionInit(kMemStart, pMemSize / 3);
    uMemHandler = dPartitionInit(uMemStart, pMemSize / 3);


	
}
