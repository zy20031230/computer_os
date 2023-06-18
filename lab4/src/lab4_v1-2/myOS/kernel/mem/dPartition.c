#include "../../include/myPrintk.h"


//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, belongs to user
	};	                           
} EMB;	//共占8个字节.

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}
unsigned long align_8(unsigned long size){
	unsigned long check;
	check = size &7;
	size = (size >> 3) <<3;
	if(check != 0 )
	size += 8;
	return size;
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	
	*/
	if(totalSize<sizeof(EMB)+sizeof(dPartition)) return 0;
	dPartition *handle = (dPartition *)start;
	handle->size = totalSize;
	handle->firstFreeStart = start + sizeof(dPartition);//第一个空闲位置
	EMB * BLOCK = (EMB *)handle->firstFreeStart;
	BLOCK->size = totalSize - sizeof(dPartition)-sizeof(EMB);//作为纯粹的指向内存块数据量的大小.也就是后面的模块中所有的内存大小,所以删去了EMB中的内容
	BLOCK->nextStart = 0;
	return start;
	

}


void dPartitionWalkByAddr(unsigned long dp){
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息

	*/

// 猜测功能为从句柄开始,逐渐打印出所有的信息
	showdPartition((dPartition *) dp);
	dPartition * handle = (dPartition *) dp;
	EMB *tobeprint;
	tobeprint =(EMB *) handle->firstFreeStart;
	while((unsigned long)tobeprint != 0){
		showEMB(tobeprint);
		tobeprint = (EMB *)tobeprint->nextStart;
	}
}


//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/
// 在这里需要保护EMB的数据域,也就是存有多少的数据量,

//一下函数返回之后的EMBsize域之后的内存,所以释放的时候进行重建需要考虑这里的问题

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。
	*/

//在每一次的分配中,至少需要分配一个EMB的大小,也就是8个字节,两个EMB之间,至少需要预留8byte,同时建立的EMB块内部
//注意,这里block块本身占用了一定内存,同时
	if(size <0) return 0;
	dPartition * handle  = (dPartition *) dp;
	EMB *block_1,*block_2,*block_new;
	size  = align_8(size);//数据量对齐操作
	unsigned long add = handle->firstFreeStart;//直接指向后面一个结构体的最初的位置
	block_1 =(EMB *) handle->firstFreeStart;
	block_2 =(EMB *) handle->firstFreeStart;
	
	int flag = 1;
	unsigned long keep_size;
	unsigned long addr;
	int first = 1;//用于区分第一个模块,也就是这里如果发生了修改一定需要修改handle
	//同时,在内部增加一个内存块不会去增加EMB块的数据结构数量,所以在这个设定中,没有必要考虑EMB占用字节,EMB设定是考虑纯大小
	while(flag){
		if(first){
			if(block_1->size > size + sizeof(unsigned long)){
				keep_size = block_1->size;
				addr = (unsigned long)block_1;
				handle->firstFreeStart = ((unsigned long)block_1)+size+sizeof(unsigned long);//下一个new的位置
				block_new = (EMB *)handle->firstFreeStart;
				block_new->size = keep_size - size-sizeof(unsigned long);
				block_new->nextStart=block_1->nextStart;//new的初始化
				block_1->size = size;//在这个数据块中占有的所有的空间
				return addr+sizeof(unsigned long);//从销毁上来看,不能对EMB结构size进行处理
			}
			else{//表示不能完成分配
				first = 0;
				block_2 = (EMB *)block_1->nextStart;
				if(block_1->nextStart == 0)return 0;//表示终止判断
			}
		}
		else{
			if(block_2->size > size + sizeof(unsigned long)){
				keep_size = block_2->size;
				addr = (unsigned long)block_2;
				block_1->nextStart = ((unsigned long)block_2) +size+sizeof(unsigned long);
				block_new =(EMB *)block_1->nextStart;
				block_new->size = keep_size - size-sizeof(unsigned long);
				block_new->nextStart=block_2->nextStart;
				block_2->size = size;
				return addr+sizeof(unsigned long);
			}
			else{
				block_1 = block_2;
				if(block_2->nextStart ==0 ) return 0;
				else block_2 = (EMB *)block_2->nextStart;
			}
		}

	}}

/*一下写法存在一定顺序错误,同时方法没有保留EMB的数据域,先保留之后查看*/
	// 	if(block_2->nextStart == 0)//在这种情况下,表示后续节点遍历结束,直接在后续链接节点即可
	// 	{
	// 		if(block_2->size < size) return 0;//size是直接作为字节数量的大小
	// 		else{
	// 			keep_size = block_2->size;
	// 			if(only_one)//表示作为只有一个EMB,需要修改firsthandle
	// 			{handle->firstFreeStart = ((unsigned long)block_2 )+ size;//也就是丢弃了第一个block_2的一个emb
	// 			block_2 = (EMB *)handle->firstFreeStart;}
	// 			else{
	// 			block_1->nextStart = ((unsigned long)block_2)+ size;
	// 			block_2 = (EMB *)block_1->nextStart;
	// 			}
	// 			//block_1表示
	// 			block_2->nextStart = 0;
	// 			block_2->size = keep_size-size;//block_2中剩余的内存
	// 			flag = 0;
	// 		}
	// 	}
	// 	else{//同样的在这种情况下
	// 		block_1 = block_2;
	// 		block_2 = (EMB *)block_2->nextStart;
	// 		//在两个EMB之间添加内容,同样的需要考虑是否是第一个EMB
	// 		if(block_1->size < size) flag = 1;
	// 		else {
	// 			keep_size = block_1->size;
	// 			if(only_one)
	// 			{
	// 				handle->firstFreeStart=((unsigned long)block_1)+size;
	// 				block_1 = (EMB *)handle->firstFreeStart;
	// 			}
	// 			else{
					
	// 			}
				
	// 		}
	// 	}
	// }



unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	*/
	dPartition *handle = (dPartition *)dp;//
	if(start < dp + sizeof(dPartition)) return 0;
	// myPrintk(0x7,"hello2\n");
	if(start > dp +handle->size) return 0;
	// myPrintk(0x7,"hello3\n");
	//以上都表示数据越界
	//以下表示相关数据的处理,每次释放一个内存空间,需要多出一个相关的EMB块对这里的数据空间进行管理,同时,如果有相邻的空间,需要对这里的空间进行合并,所以新建立的EMB块中的数据必须打上相关的标记,表示这里的空间已经被释放
	unsigned long addr = handle->firstFreeStart;
	unsigned long addr_pre = 0;
	unsigned long addr_next = 0;
	EMB *block,*block_next;
	//之前的分配,每一次会留下一个size域
	start = start - sizeof(unsigned long);//指向保留的EMB的数据域,同时start如果被直接写入会保留在next域上面,所以说不能显示出来
	while (addr)
	{
		block = (EMB *)addr;
		if(addr < start)
		addr_pre = addr;
		else if(addr >start)
		{
			addr_next = addr;
			break;
		}
		addr = block->nextStart;
		/* code */
	}

	block = (EMB *)start;//可以拿到block的size域
	block->nextStart = 0;//对于block的初始化,同时size也需要修改,具体的策略在下方

	if(addr_next){
		if(addr_next == start + block->size + sizeof(unsigned long)){//相邻且需要合并
			block_next = (EMB *)addr_next;
			block->size  = block->size +block_next->size+sizeof(unsigned long);
			block_next->nextStart = block_next->nextStart;

		}
		else {block->nextStart = addr_next;
		block->size=block->size+sizeof(unsigned long)-sizeof(EMB);
		}
	}
	else{
		block->size=block->size+sizeof(unsigned long)-sizeof(EMB);//注意到这里的EMB已经是正常的一个EMB数据结构.
	}

// if(addr_pre == 0) myPrintk(0x7,"0\n");
// else myPrintk(0x7,"1\n");

	if(addr_pre){//无就改头,有要么合并要么改指针,同时不需要去修改的时候对于前面的存储段基本上没有什么影响
		block_next = (EMB*)addr_pre;
		if(start == addr_pre + block_next->size + sizeof(EMB)){
			block_next->size = block_next->size+block->size+sizeof(EMB);
			block_next->nextStart = block->nextStart;			
		}
		else block_next->nextStart = start;
	}
	else 
	handle->firstFreeStart = start;//
return 1;
	
}


// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
