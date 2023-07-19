#ifndef __TASK_H__
#define __TASK_H__

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#endif

//需要新增的一些函数文件,来实现开关中断的操作.
#include "i8253.h"
#include "i8259.h"
#include "io.h"
#include "irq.h"
#define TASK_NUM (8 + USER_TASK_NUM)   // at least: 0-idle, 1-init ,以防万一这里多申请了几个

#define initTskBody myMain         // connect initTask with myMain

#define STACK_SIZE 512            // definition of STACK_SIZE

//对于调度器的实现
#define FIFO 0
#define PRIO 1
#define RR 2



#define max_prio 10
#define max_time 10 //最长的执行时间

#define priority 0
#define exec_time 1
#define arrive_time 2

//相关HOOK的实现

int idle_flag ;
typedef struct taskpara
{
     int prio;
     int time_need;
     int time_arrive;
} taskpara;
int debug;
void initTskBody(void);

void CTX_SW(void*prev_stkTop, void*next_stkTop);

//#error "TODO: 为 myTCB 增补合适的字段"
typedef struct myTCB {
     unsigned long *stkTop;        /* 栈顶指针 */
     unsigned long stack[STACK_SIZE];      /* 开辟了一个大小为STACK_SIZE的栈空间 */  
     unsigned long TSK_State;   /* 进程状态 */
     unsigned long TSK_ID;      /* 进程ID */ 
     void (*task_entrance)(void);  /*进程的入口地址*/
     struct myTCB * nextTCB;           /*下一个TCB,这个tcb用在arri上*/
     struct myTCB * sche_TCB;
     //对于lab6的新增的功能
     struct myTCB * poolnext; //进程池中的下一个
     unsigned long runtime; //任务执行时间
     unsigned long lasttime;//上一次的调度的时间
     taskpara *para;//这里是附带的信息
} myTCB;

myTCB tcbPool[TASK_NUM];//进程池的大小设置

myTCB * idleTsk;                /* idle 任务 */
myTCB * currentTsk;             /* 当前任务 */
myTCB * firstFreeTsk;           /* 下一个空闲的 TCB */

//调度器的选择采取助教给的模式
typedef struct scheduler
{
     unsigned long type;//调度类型
     int preemptive_or_not;
     myTCB *(*nextTsk)(void);
     myTCB *(*dequeuetsk_func)(void);
     void (*enqueuetsk_func)(myTCB *tsk);//相当于之后还是直接传入一级指针.
     // void (*dequeuetsk_func)(myTCB *tsk);
     void (*scheduleinit_func)(myTCB *idleTsk);
     void (*schedule)(void);
     void(*tick_hook)(void);//时钟的信号

}scheduler;


void init_sche(void);//选择的创建
void TaskManagerInit(void);

//taskpara设置的.h文件出处
int sys_type(void);
void tskpara_init(taskpara **para);
void tskpara_set(unsigned int option,unsigned int value,taskpara *para);
unsigned int tskpara_getinfo(unsigned option,taskpara *para);//用来返回para中对应的数据的函数
void show_info(void);

void tskpara_exe();
//以下定义优先级调度参变量,仿照lab5的fifo设计
typedef struct taskQueprio
{
     myTCB *tcb;//类似于一个头节点,之后放在tcb的表上,这里还是需要进行比较的,可能出现队列被放在最前面的情况,在这里为了避免随意的使用kmalloc,故选择不采用设置头节点的方式
     myTCB *idletsk;
     int tail;
     int (*cmp)(const myTCB *a,const myTCB *b);  
}taskQueprio;



void tsk_in_arr(myTCB * tsk);
void tskpara_set(unsigned int option,unsigned int value,taskpara *para);
void rqFCFSInit(myTCB* idleTsk);
int rqFCFSIsEmpty(void);
myTCB * nextFCFSTsk(void) ;
void tskEnqueueFCFS(myTCB *tsk);
void tskDequeueFCFS(myTCB *tsk);
void scheduleFCFS(void);
int count;
void taskprio_init(myTCB *idletsk,int(*cmp)(const myTCB *a,const myTCB *b));
void taskprio_init_real(myTCB *idleTsk);
int  taskprio_empty();
myTCB * taskprio_next();
void taskprio_enqueue(myTCB *tsk);
myTCB *taskprio_dequeue();
void sche_prio(void);

// RR时间片轮转调度
void RR_Init(myTCB *idleTsk);
int RR_IsEmpty(void);
myTCB * RR_nextTsk(void);
void RR_tskEnqueue(myTCB *tsk);
myTCB *RR_tskDequeue(void) ;
void RR_sche(void);
void tskend_RR();
#define TIMESLICE 2
//上下文切换
void context_switch(myTCB *prevTsk, myTCB *nextTsk);
void tskStart(myTCB *tsk);
void tskEnd_prio(void);

//for debug
int shownum(void);
#endif
