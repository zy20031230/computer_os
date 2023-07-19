#include "../include/task.h"
#include "../include/myPrintk.h"
#include "../include/wallClock.h"
#include "../include/kmalloc.h"
//之后应该是需要对比一下函数的名字,很奇怪的是,这里的任务的切换到底是做了什么样的操作
//下一个任务需要使用的是离队的任务还是next_tcb????
void schedule(void);
void destroyTsk(int takIndex);

myTCB  *next_tsk_prio;
myTCB  *next_tsk_RR;
#define TSK_RDY 0        //表示当前进程已经进入就绪队列中
#define TSK_WAIT -1      //表示当前进程还未进入就绪队列中
#define TSK_RUNING 1     //表示当前进程正在运行
#define TSK_NONE 2       //表示进程池中的TCB为空未进行分配

#define NULL (void *)0
//以下申请HOOK的函数名


//以上是一个悬挂的函数
scheduler sche_sys;
taskQueprio rdy_prio;
int sys_type(void){
     return sche_sys.type;
}
void init_sche(){
     switch (sche_sys.type)
     {
     case FIFO :
          sche_sys.scheduleinit_func = rqFCFSInit;
          sche_sys.enqueuetsk_func  = tskEnqueueFCFS;
          // sche_sys.dequeuetsk_func = tskDequeueFCFS;
          sche_sys.nextTsk = nextFCFSTsk;
          sche_sys.tick_hook = NULL;//也就是在这个的算法中是不需要时钟机制的
          sche_sys.preemptive_or_not = 0;//这里表示先进先出是不需要进行抢占的
          sche_sys.schedule = scheduleFCFS;
          break;
     case PRIO :
          sche_sys.scheduleinit_func = taskprio_init_real;
          sche_sys.enqueuetsk_func = taskprio_enqueue;
          sche_sys.dequeuetsk_func = taskprio_dequeue;
          sche_sys.nextTsk = taskprio_next;
          sche_sys.tick_hook = NULL;//实际上这里没有对于这个函数完成操作
          sche_sys.preemptive_or_not = 0;//这里也没有对这个东西进行操作
          sche_sys.schedule = sche_prio; //调度函数机制.
     break;
     case RR:
          sche_sys.scheduleinit_func = RR_Init;
          sche_sys.enqueuetsk_func = RR_tskEnqueue;
          sche_sys.dequeuetsk_func = RR_tskDequeue;
          sche_sys.nextTsk = RR_nextTsk;
          sche_sys.tick_hook = NULL ;//实际上没有任何作用
          sche_sys.preemptive_or_not = 1;//实际上也没有任何作用
          sche_sys.schedule = RR_sche;
          break;
     default:
          break;
     }                        
}
//tskIdleBdy进程（无需填写）
void tskIdleBdy(void) {

     // myPrintk(0x7,"************idle************");
     while(1){ //这里应该只是一个简单的调度.
          next_tsk_prio = sche_sys.nextTsk();
          if(next_tsk_prio!=idleTsk) context_switch(currentTsk,next_tsk_prio);
     }

}
//时钟信号的处理


void current_time_add(void){
     disable_interrupt();
     currentTsk->runtime++;
     enable_interrupt();//这个之后需要悬挂在时钟项目上面,

}
//以下的这些函数都是在做把相关的task在特定的时间拿出来在放入进去
//按到来时间排列的函数,这个链表的实现原理是
taskQueprio que_arr;
int compare_arr(const myTCB *a,const myTCB *b){
     return tskpara_getinfo(arrive_time,a->para)- tskpara_getinfo(arrive_time,b->para);
}




void tsk_arri_enqueue(myTCB *tsk){//这里相当于是直接通过进城池进行选择
     int i; 
     myTCB *next = que_arr.tcb;
     myTCB *pre=NULL;
     disable_interrupt();//关中断
     if(que_arr.tail==0)
     que_arr.tcb = tsk;
     else{

          for(i=0;i<que_arr.tail;i++)
          {
               if(que_arr.cmp(next,tsk)<0)//表示next的优先级更高,所以向后面排列 tsk>next就会向后面排列
               {
                    pre = next;
                    next = next->nextTCB;
               }
               
          }//终止循环了就表示next的优先级更低,但是pre的优先级更高,同样的也就是把tsk插入在pre的后面,所有存在两种情况pre==NULL以及pre有正常的值
     }    
     if(pre==NULL)
     {
          pre = que_arr.tcb;//保留原来的对头元素
          que_arr.tcb = tsk;//把新的东西作为新的值
          que_arr.tcb->nextTCB = pre;
     }
     else {
          tsk->nextTCB = pre->nextTCB;
          pre->nextTCB = tsk;
     }
     que_arr.tail++;//数量加1
     enable_interrupt();//开中断
}

myTCB *tsk_arri_dequeue(){
     if(que_arr.tail==0)
     {
          que_arr.tcb=NULL;
          return NULL;
     }
     que_arr.tail--;//个数减一
     if(que_arr.tail==0)
     {
          que_arr.tcb=NULL;
          return NULL;
     }
     disable_interrupt();
     myTCB *task = que_arr.tcb;//队头元素
     que_arr.tcb = task->nextTCB;//接上下一个元素
     task->nextTCB = NULL;
     enable_interrupt();
     return task;
}
//这里明显的是选择一开始对于所有队列进行排队,然后到了特定的时间在把相应的队列拿出来放在调度算法中排队,所以这里如果需要支持抢占的话,需要有一种操作
//把当前的队列放进去进行排队.

void tsk_arri_hook(void){

     if(que_arr.tail==0)//表示空队列
     return;

     myTCB *tsk_next = que_arr.tcb;//拿到第一个函数
     disable_interrupt();
     if(second_get()>=tskpara_getinfo(arrive_time,tsk_next->para) && tsk_next!=NULL){//表示tsk是真实存在的
          tsk_arri_dequeue();//选择离队
          tsk_next->nextTCB = NULL; //维护之后的链表
          tskStart(tsk_next);//用这里的语句进行调度 这里应该也是错误的
     }
     enable_interrupt();
}


//初始化任务到达的函数
void tsk_in_arr(myTCB * tsk){
     if(tsk->para->time_arrive==0)
     tskStart(tsk);
     else tsk_arri_enqueue(tsk);
     //表示如果执行时间是0,那么直接放进队列进行执行,否则等待
}
void task_arri_init(myTCB *idletsk,int(*compare_arr)(const myTCB *a,const myTCB *b)){
     que_arr.idletsk = idleTsk;//空闲的任务,也就是没有任务的常态
     que_arr.tail = 0;
     que_arr.tcb = NULL;
     que_arr.cmp = compare_arr;
     append_hook(tsk_arri_hook);
     }//按照时间的方式进行排队的函数



//一些选择的操作------------------------------------------------------
extern int startflag;//

void tskpara_init(taskpara **para){
     *para = (taskpara *)kmalloc(sizeof(taskpara));
     (*para)->prio = 0;
     (*para)->time_arrive = 0;
     (*para)->time_need = 0;
}//定义的这个函数可以在里面直接完成相关内容的定义

//维护值的输入
void tskpara_set(unsigned int option,unsigned int value,taskpara *para){
     switch (option)   
     {
     case priority:
          para->prio = value > max_prio?max_prio:value;
          /* code */
          break;
     case exec_time:
          para->time_need = value >max_time?max_time:value;
          break;
     
     case arrive_time:
          para->time_arrive = value;
          break;
     
     default:
          break;
     }
}
unsigned int tskpara_getinfo(unsigned option,taskpara *para){
     switch (option)
     {
     case priority:
          return para->prio;//所选择的方式是通过指针的形式完成.
     
     case exec_time:
          return para->time_need;
     
     case arrive_time:
          return para->time_arrive;
     
     default:
          break;
     }
}
//这里应该是挂载的函数正在进行.
void tskpara_exe(){
     //任务执行的函数
     while(currentTsk->runtime <tskpara_getinfo(exec_time,currentTsk->para)*100);

}

void show_info(){
     // if(debug == 20)myPrintk(0x7,"111111111111");

     if(kernel_time() % 100 != 0) return;
     if(currentTsk == idleTsk) {
     myPrintk(0x7,"**********idle_now*******\n"); return;}
     if(sche_sys.type==PRIO)
     {
     myPrintk(0x7,"*************************\n");

     myPrintk(0x7,"currentid :%d            \n",currentTsk->TSK_ID);
     myPrintk(0x7,"priority: %d            \n",tskpara_getinfo(priority,currentTsk->para));
     // myPrintk(0x7,"first id: %d            \n",rdy_prio.tcb->TSK_ID);
     myPrintk(0x7,"Execute time: %d/%d     \n",(currentTsk->runtime/100),tskpara_getinfo(exec_time,currentTsk->para));
     myPrintk(0x7,"************prio ********\n");
     }
     if(sche_sys.type==RR){
          myPrintk(0x7,"*************************\n");

          myPrintk(0x7,"currentid :%d            \n",currentTsk->TSK_ID);
          myPrintk(0x7,"Execute time: %d/%d     \n",((currentTsk->runtime +20) /100),tskpara_getinfo(exec_time,currentTsk->para));
          myPrintk(0x7,"************ RR ********\n");
     }
     // for_debug----------

     // myPrintk(0x7,"priority1: %d            \n",tskpara_getinfo(priority,rdy_prio.tcb->para));
     // if(rdy_prio.tcb->sche_TCB->para!=NULL) debug =1;
     // else
     // myPrintk(0x7,"priority2: %d            \n",tskpara_getinfo(priority,rdy_prio.tcb->sche_TCB->para));
     // myPrintk(0x7,"priority3: %d            \n",tskpara_getinfo(priority,rdy_prio.tcb->sche_TCB->sche_TCB->para));
}
//这个表示为执行时间,如果还在执行中,选择在while循环中执行.
//对于info函数,必须得到程序中的时钟值,否则无法完成执行.
// ------for prio 调度的相关函数--------


int comp_prio(const myTCB *a,const myTCB *b){
     // if(a!=NULL && b!=NULL){
     if(tskpara_getinfo(priority,a->para)==tskpara_getinfo(priority,b->para))
     {
          return tskpara_getinfo(arrive_time,a->para)- tskpara_getinfo(arrive_time,b->para);

     }
     else 
          return tskpara_getinfo(priority,a->para) - tskpara_getinfo(priority,b->para);
     // }
     // else if (a==NULL) return -1;
     // else if (b==NULL) return 1;
}

void taskprio_init(myTCB *idletsk,int(*cmp)(const myTCB *a,const myTCB *b)){
     rdy_prio.idletsk = idleTsk;//空闲的任务,也就是没有任务的常态
     rdy_prio.tcb = NULL;
     rdy_prio.tail = 0;
     rdy_prio.cmp = cmp;
}
void taskprio_init_real(myTCB *idleTsk){
     taskprio_init(idleTsk,comp_prio);
}

int  taskprio_empty(){
     //这里表示如果尾端是0,返回1,表示空队列
     return rdy_prio.tail ==0;
}

myTCB * taskprio_next(){
     if(taskprio_empty())
     return rdy_prio.idletsk;
     else return rdy_prio.tcb;//也就是第一个节点
     
}
void taskprio_enqueue(myTCB *tsk){//这里相当于是直接通过进城池进行选择
     int i; 
     tsk->sche_TCB =NULL;

     myTCB *next = rdy_prio.tcb;
     myTCB *pre=NULL;
     disable_interrupt();//关中断
     if(taskprio_empty())
     rdy_prio.tcb = tsk;
     else{
          for(i=0;i<rdy_prio.tail;i++)
          {
               if(rdy_prio.cmp(next,tsk)<=0)//next - tsk <0 -> tsk > next tsk的优先级靠后向后面排列
               {
                    pre = next;
                    next = next->sche_TCB;
               }
               
          }//终止循环了就表示next的优先级更低,但是pre的优先级更高,同样的也就是把tsk插入在pre的后面,所有存在两种情况pre==NULL以及pre有正常的值
         
     if(pre==NULL)
     {
          pre = rdy_prio.tcb;//保留原来的对头元素
          rdy_prio.tcb = tsk;//把新的东西作为新的值
          rdy_prio.tcb->sche_TCB = pre;
     }
     else {
          tsk->sche_TCB = pre->sche_TCB;
          pre->sche_TCB = tsk;
     }
     }
     rdy_prio.tail++;//数量加1
     enable_interrupt();//开中断
}

myTCB *taskprio_dequeue(){
     if(taskprio_empty()){
          rdy_prio.tcb=NULL;
     // return rdy_prio.idletsk;
     return NULL;
     }
     myTCB *pre_prio = NULL;
     myTCB *task = rdy_prio.tcb;//队头元素
     int deq_flag = 0;
     disable_interrupt();
     int i;
     for(i=0;i<rdy_prio.tail;i++){
          if(currentTsk == rdy_prio.tcb){
               rdy_prio.tcb = currentTsk->sche_TCB;
               deq_flag = 1;
          }//下面表示不是首元素
          else if(currentTsk != task){
               pre_prio = task;
               task = task->sche_TCB;
          }
     }
     rdy_prio.tail--;//个数减一
     if(deq_flag==0){
          pre_prio->sche_TCB = task->sche_TCB;

     }
     // rdy_prio.tcb = task->sche_TCB;//接上下一个元素
     task->sche_TCB = NULL;
     // if(taskprio_empty()) rdy_prio.tcb = NULL;
     enable_interrupt();
     return task;
}

void sche_prio(void){
     // myTCB  *next_tsk_prio;
          next_tsk_prio = taskprio_next();
          next_tsk_prio->TSK_State = TSK_RUNING;
          context_switch(currentTsk,next_tsk_prio);

     }


void tskEnd_prio(void){
     taskprio_dequeue();//
     destroyTsk(currentTsk->TSK_ID);
     sche_prio();
}


// -------- end prio------------
//tskEmpty进程（无需填写）
void tskEmpty(void){
}
// ---------FIFO---------------------
//就绪队列的结构体
typedef struct rdyQueueFCFS{
     myTCB * head;
     myTCB * tail;
     myTCB * idleTsk;//这个有什么用?
} rdyQueueFCFS;

rdyQueueFCFS rqFCFS;

//初始化就绪队列（需要填写）
void rqFCFSInit(myTCB* idleTsk) {//对rqFCFS进行初始化处理 IDLE在这里怎么去合理的处理?

     rqFCFS.head = rqFCFS.tail  = NULL;
     rqFCFS.idleTsk = idleTsk;
}

//如果就绪队列为空，返回True（需要填写）
int rqFCFSIsEmpty(void) {//当head和tail均为NULL时，rqFCFS为空
     int flag;
     flag = (rqFCFS.head == NULL && rqFCFS.tail ==NULL);
     return flag;
}

//获取就绪队列的头结点信息，并返回（需要填写）
myTCB * nextFCFSTsk(void) {//获取下一个Tsk
     if(rqFCFSIsEmpty())
     return rqFCFS.idleTsk;//如果是空的则说明是执行一个idletsk
     else return rqFCFS.head;
}

//将一个未在就绪队列中的TCB加入到就绪队列中（需要填写）
void tskEnqueueFCFS(myTCB *tsk) {//将tsk入队rqFCFS
     if(rqFCFSIsEmpty()){
          //表示里面都是空的
          rqFCFS.head=rqFCFS.tail = tsk;
     }
     else{
          rqFCFS.tail->sche_TCB = tsk;
          rqFCFS.tail = tsk;
     }
}

//将就绪队列中的TCB移除（需要填写）
void tskDequeueFCFS(myTCB *tsk) {//rqFCFS出队
//这里为什么需要填入tsk,而不是直接出队?
     if(rqFCFSIsEmpty())
     return;
     else {
          rqFCFS.head = rqFCFS.head->sche_TCB;
     }
}
//--------------------时间片轮转调度------------------------RRRRRRRRR---------------------------------//
//感觉理论上和FIFO差不多??如果时间到了就直接把他丢出队列,时间没到就加入队列,大概是这么操作?
rdyQueueFCFS rdy_RR;

void RR_Init(myTCB *idleTsk) {
     rdy_RR.head = rqFCFS.tail  = NULL;
     rdy_RR.idleTsk = idleTsk;
}

int RR_IsEmpty(void) {//当head和tail均为NULL时，rqFCFS为空
     int flag;
     flag = (rdy_RR.head == NULL && rdy_RR.tail ==NULL);
     return flag;
}
myTCB * RR_nextTsk(void) {//获取下一个Tsk
     if(RR_IsEmpty())
     return rdy_RR.idleTsk;//如果是空的则说明是执行一个idletsk
     else return rdy_RR.head;
}

void RR_tskEnqueue(myTCB *tsk) {//将tsk入队rqFCFS
     count++;
     if(RR_IsEmpty()){
          //表示里面都是空的
          rdy_RR.head=rdy_RR.tail = tsk;
     }
     else{
          rdy_RR.tail->sche_TCB = tsk;
          rdy_RR.tail = tsk;
     }
}

myTCB *RR_tskDequeue(void) {//rqFCFS出队
//这里为什么需要填入tsk,而不是直接出队?
     disable_interrupt();
     count--;
     if(RR_IsEmpty())
     { return rdy_RR.idleTsk; 
     return NULL;}

     else {
          myTCB *tsk;
          tsk = rdy_RR.head;
          rdy_RR.head = rdy_RR.head->sche_TCB;
          return tsk;
     }
     disable_interrupt();
}
//主要是调度的处理怎么来选择相关的调度信息就比较重要
void RR_sche(void){
     next_tsk_RR = RR_nextTsk();
     next_tsk_RR->TSK_State = TSK_RUNING;
     context_switch(currentTsk,next_tsk_RR);
}
void RR_sche_hook(void){
     if(currentTsk == idleTsk) return;
     myTCB *RR_next;
     RR_next = RR_nextTsk();//如果是下个任务直接就是idletsk则没有必要进行转换
     if(currentTsk->runtime %(TIMESLICE * 100) ==0 && RR_next!=idleTsk){
          //表示时间已经到了
          currentTsk->TSK_State = TSK_RDY;
          RR_tskDequeue();
          RR_tskEnqueue(RR_next);
          context_switch(currentTsk,RR_next);
     }
}

void tskend_RR(){
     RR_tskDequeue();
     destroyTsk(currentTsk->TSK_ID);
     RR_sche();
}

//初始化栈空间（不需要填写）
void stack_init(unsigned long **stk, void (*task)(void)){
     *(*stk)-- = (unsigned long) 0x08;       //高地址
     *(*stk)-- = (unsigned long) task;       //EIP
     *(*stk)-- = (unsigned long) 0x0202;     //FLAG寄存器

     *(*stk)-- = (unsigned long) 0xAAAAAAAA; //EAX
     *(*stk)-- = (unsigned long) 0xCCCCCCCC; //ECX
     *(*stk)-- = (unsigned long) 0xDDDDDDDD; //EDX
     *(*stk)-- = (unsigned long) 0xBBBBBBBB; //EBX

     *(*stk)-- = (unsigned long) 0x44444444; //ESP
     *(*stk)-- = (unsigned long) 0x55555555; //EBP
     *(*stk)-- = (unsigned long) 0x66666666; //ESI
     *(*stk)   = (unsigned long) 0x77777777; //EDI

}

//进程池中一个未在就绪队列中的TCB的开始（不需要填写）


//lab5的生命流程从tskstart中开始,放置成RDY, 然后放进队列
//而创建的进程是用creat开始,完成相关的初始化后,调用tskstart,放入就绪队列中返回一个TID,里面有函数名

//通过传递进程的地址开始创建,这里是把任务在特定的时间里传入队列来进行完成.
void tskStart(myTCB *tsk){
     tsk->TSK_State = TSK_RDY;
     //将一个未在就绪队列中的TCB加入到就绪队列
     // tskEnqueueFCFS(tsk);
     sche_sys.enqueuetsk_func(tsk);//把这个放进就绪队列等待调整.


}

//进程池中一个在就绪队列中的TCB的结束（不需要填写）
void tskEnd(void){
     //将一个在就绪队列中的TCB移除就绪队列
     //  tskDequeueFCFS(currentTsk);
     // sche_sys.dequeuetsk_func();
     //由于TCB结束，我们将进程池中对应的TCB也删除
     //  destroyTsk(currentTsk->TSK_ID);
     //TCB结束后，我们需要进行一次调度
     // schedule();
}
//prio 进程的结束


//以tskBody为参数在进程池中创建一个进程，并调用tskStart函数，将其加入就绪队列（需要填写）

// 从之后的过程可以看出来,进程是从这里的进去的

int createTsk(void (*tskBody)(void)){//在进程池中创建一个进程，并把该进程加入到rqFCFS队列中
     if(!firstFreeTsk) return -1;//这个应该是在进程的创造之初就完成了链表化,之后销毁也是在其之后直接连接上下一个TCB
     myTCB * tsk_new = firstFreeTsk;
     firstFreeTsk = firstFreeTsk->poolnext;
     tsk_new->task_entrance = tskBody;

     tsk_new->runtime = tsk_new->lasttime = 0;
     stack_init(&tsk_new->stkTop,tskBody);
     // tskStart(tsk_new);    //这里不能直接让任务开始,否则无法按时间顺序进行排列
     return tsk_new->TSK_ID;

}

//以takIndex为关键字，在进程池中寻找并销毁takIndex对应的进程（需要填写）
void destroyTsk(int takIndex) {//在进程中寻找TSK_ID为takIndex的进程，并销毁该进程
     //栈顶置针复位
     myTCB * thisTCB;
     thisTCB = &tcbPool[takIndex];
     thisTCB->task_entrance = NULL;
     thisTCB->TSK_State = TSK_NONE;
     thisTCB->stkTop = thisTCB->stack+STACK_SIZE-1;
     thisTCB->nextTCB = NULL;
     thisTCB->poolnext = firstFreeTsk;
     thisTCB->runtime = 0;
     thisTCB->lasttime = 0;
     thisTCB->para = NULL;//para的初始化,奇怪的是这里选择了使用指针的类型进行操作.
     firstFreeTsk = thisTCB;
}
// TCB切换,也就是把已经拿出来的任务销毁.

unsigned long **prevTSK_StackPtr;
unsigned long *nextTSK_StackPtr;

//切换上下文（无需填写）这里包含了两个进程之间切换.


void context_switch(myTCB *prevTsk, myTCB *nextTsk) {
     prevTSK_StackPtr = &(prevTsk->stkTop);
     currentTsk = nextTsk;
     nextTSK_StackPtr = nextTsk->stkTop;
     CTX_SW(prevTSK_StackPtr,nextTSK_StackPtr);
}
//进程切换包括了tcb的切换 以及相关栈的

//FCFS调度算法（无需填写）
void scheduleFCFS(void) {
     myTCB *nextTsk;
     nextTsk = nextFCFSTsk();
     context_switch(currentTsk,nextTsk);
}

//调度算法（无需填写）

//这里相当于一个直接的封装
void schedule(void) {
     // scheduleFCFS();

     sche_sys.schedule();//相应模式的调度算法.
     
}

//进入多任务调度模式(无需填写)
//抽象,当前的进程是怎么优先执行的
unsigned long BspContextBase[STACK_SIZE];
unsigned long *BspContext;
void startMultitask(void) {
     BspContext = BspContextBase + STACK_SIZE -1;
     prevTSK_StackPtr = &BspContext;
     
     // currentTsk = nextFCFSTsk();
     currentTsk = sche_sys.nextTsk();//表示是当前队列的第一个函数
     nextTSK_StackPtr = currentTsk->stkTop;
     CTX_SW(prevTSK_StackPtr,nextTSK_StackPtr);//直接开始处理进程
     schedule();
}



//注意到每次添加一个元素用lab5的框架可能会出现错误,所以在下面额外添加一个pool指针,用以指向下一个池子的位置,pool需要良好的维护习惯
//nexttcb用来维护剩下的指针向 所以之后需要修改原来的FCFS的项目运行方式


//准备进入多任务调度模式(无需填写)
void TaskManagerInit(void) {
     // 初始化进程池（所有的进程状态都是TSK_NONE）
     int i;
     myTCB * thisTCB;
     for(i=0;i<TASK_NUM;i++){//对进程池tcbPool中的进程进行初始化处理
          thisTCB = &tcbPool[i];
          thisTCB->TSK_ID = i;
          thisTCB->stkTop = thisTCB->stack+STACK_SIZE-1;//将栈顶指针复位
          thisTCB->TSK_State = TSK_NONE;//表示该进程池未分配，可用
          thisTCB->task_entrance = tskEmpty;
          tskpara_init(&thisTCB->para);//选择指针的形式来进行初始化也就是最开始的时候就选择做了para的初始化,也就是其对应的指针可以指向特定地值.
          thisTCB->para->time_need = 0;
          thisTCB->runtime = 0;
          thisTCB->lasttime = 0;
          thisTCB->nextTCB =NULL;
          thisTCB->sche_TCB =NULL;
          if(i==TASK_NUM-1){
               thisTCB->poolnext = (void *)0;
          }
          else{
               thisTCB->poolnext = &tcbPool[i+1];
          }//TCB已经完全完成分配在tcb是一个链接起来的池子.
     }
     myPrintk(0x7,"hello");
     sche_sys.type = PRIO ; //从这里选择 表示进程类型是PRIO的
     init_sche();//创建

     task_arri_init(idleTsk,compare_arr);//时钟队列的创建
     count = 0;
     //创建idle任务
     idleTsk = &tcbPool[0];
     stack_init(&(idleTsk->stkTop),tskIdleBdy);
     idleTsk->task_entrance = tskIdleBdy;//这里也就是tskidlebdy是一个函数一直在做调度
     idleTsk->nextTCB = (void *)0;
     idleTsk->poolnext = NULL;
     idleTsk->TSK_State = TSK_RDY;
     // rqFCFSInit(idleTsk);
     sche_sys.scheduleinit_func(idleTsk);//IDLE的创建
     //idle任务的创建与
     firstFreeTsk = &tcbPool[1];//拿到第一个任务
     debug = 0;
     //创建init任务
     createTsk(initTskBody);//在这里创建mymain的函数状态的,之后用这进入主函数.
     tskStart(&tcbPool[1]);//从这里开始最初的任务并且直接启动.
     enable_interrupt();//开中断
     append_hook(current_time_add);
     append_hook(show_info);//
     if(sche_sys.type==RR) append_hook(RR_sche_hook);//RR抢占类型的悬挂

     //进入多任务状态
     myPrintk(0x2,"START MULTITASKING......\n");
     startMultitask();
     myPrintk(0x2,"STOP MULTITASKING......SHUT DOWN\n");

}


// for debug
int shownum(void){
     // return rdy_RR.tail;
     return rdy_RR.head->sche_TCB->TSK_ID;
}