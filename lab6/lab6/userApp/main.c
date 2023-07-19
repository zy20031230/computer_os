#include "../myOS/userInterface.h"   //interface from kernel
#include "shell.h"
#include "memTestCase.h"

void myTsk0(void);
void myTsk1(void);
void myTsk2(void);
void initPrioCases(void);
void initRRCases(void);
void startShell(void);
void initShell(void);
void memTestCaseInit(void);
int createTsk(void (*tskBody)(void));
void tskEnd(void);

void wallClock_hook_main(void){
	int _h, _m, _s;
	char hhmmss[]="hh:mm:ss\0\0\0\0";

	getWallClock(&_h,&_m,&_s);
	sprintf(hhmmss,"%02d:%02d:%02d",_h,_m,_s);//下面的语句是为了测试时钟项目,发现其是合理的.
     // sprintf(hhmmss,"%02d:%02d:%02d",second_get(),_m,_s);
	put_chars(hhmmss,0x7E,24,72);
}

void doSomeTestBefore(void){		
	setWallClock(18,59,59);		//set time 18:59:59
    	setWallClockHook(&wallClock_hook_main);
}

// init task 入口
void myMain(void) {
     clear_screen();

     //myPrintf(0x2,"debug_myMain_begin......\n");

     doSomeTestBefore();

     myPrintf(0x07, "********************************\n");
	myPrintf(0x07, "*         INIT   INIT !        *\n");
	myPrintf(0x07, "**************HELLO******************\n");

     // createTsk(myTsk0);
     // createTsk(myTsk1);
     // createTsk(myTsk2);
     if(sys_type()==PRIO)
     initPrioCases();//初始化最初的任务
     if(sys_type()==RR){
     initRRCases();
     }
     //#error "TODO: 初始化 shell 并创建 shell task"
     // myPrintk(0x7,"num:%d\n",count);
     initShell();
     memTestCaseInit();
     // createTsk(startShell);//这里应该可以考虑去直接注释掉的,否则会进入一个死循环. 
     myPrintf(0x07, "**************HELLO******************\n");
     //myPrintf(0x2,"debug_myMain_end......\n");
     if(sys_type()==PRIO)
     tskEnd_prio();
     if(sys_type()==RR){

     tskend_RR();
     }
     // tskEnd();//tskend到底需要执行多少的功能
}
