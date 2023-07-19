#include "../myOS/userInterface.h"
#define WHITE 0x7
extern void tskEnd(void);
extern int createTsk(void (*tskBody)(void));
extern int myPrintf(int color,const char *format, ...);

char *message1 = "********************************\n";
void myTskPrio0(void) {
    tskpara_exe();
	tskEnd_prio();
}

void myTskPrio1(void) {
    tskpara_exe();
	tskEnd_prio();
}

void myTskPrio2(void) {
    tskpara_exe();
	tskEnd_prio();
}
void initPrioCases(void) {
    int newTskTid0 = createTsk(myTskPrio0);
    tskpara_set(priority, 4, tcbPool[newTskTid0].para);
    tskpara_set(arrive_time, 4, tcbPool[newTskTid0].para);
    tskpara_set(exec_time, 7, tcbPool[newTskTid0].para); 

    int newTskTid1 = createTsk(myTskPrio1); 
    tskpara_set(priority, 3, tcbPool[newTskTid1].para);
    tskpara_set(arrive_time, 5, tcbPool[newTskTid1].para);
    tskpara_set(exec_time, 6, tcbPool[newTskTid1].para); 
	
	int newTskTid2 = createTsk(myTskPrio2); 
    tskpara_set(priority, 2, tcbPool[newTskTid2].para);
    tskpara_set(arrive_time, 7, tcbPool[newTskTid2].para);
    tskpara_set(exec_time, 7, tcbPool[newTskTid2].para);

	tsk_in_arr(&tcbPool[newTskTid0]);
	tsk_in_arr(&tcbPool[newTskTid1]);
	tsk_in_arr(&tcbPool[newTskTid2]);

//以此为分界线,上面的三个语句是采用到达时间的设置, 下面的三个语句是直接放入队列中的

	// tskStart(&tcbPool[newTskTid0]);
	// tskStart(&tcbPool[newTskTid1]);
	// tskStart(&tcbPool[newTskTid2]);


}

void myTskRR0(void) {
    tskpara_exe();
	tskend_RR();
}

void myTskRR1(void) {
    tskpara_exe();
	tskend_RR();
}

void myTskRR2(void) {
    tskpara_exe();
	tskend_RR();
}

void initRRCases(void) {
    int newTskTid0 = createTsk(myTskPrio0); // id:2
    tskpara_set(priority, 4, tcbPool[newTskTid0].para);
    tskpara_set(arrive_time, 4, tcbPool[newTskTid0].para);
    tskpara_set(exec_time, 7, tcbPool[newTskTid0].para); 

    int newTskTid1 = createTsk(myTskPrio1); // id: 3
    tskpara_set(priority, 3, tcbPool[newTskTid1].para);
    tskpara_set(arrive_time, 5, tcbPool[newTskTid1].para);
    tskpara_set(exec_time, 6, tcbPool[newTskTid1].para); 
	
	int newTskTid2 = createTsk(myTskPrio2); // id :4
    tskpara_set(priority, 2, tcbPool[newTskTid2].para);
    tskpara_set(arrive_time, 7, tcbPool[newTskTid2].para);
    tskpara_set(exec_time, 7, tcbPool[newTskTid2].para);

	// tsk_in_arr(&tcbPool[newTskTid0]);
	// tsk_in_arr(&tcbPool[newTskTid1]);
	// tsk_in_arr(&tcbPool[newTskTid2]);
	tskStart(&tcbPool[newTskTid0]);
	tskStart(&tcbPool[newTskTid1]);
	tskStart(&tcbPool[newTskTid2]);


}

//按照314的顺序排列

void myTsk0(void){      
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk0: HELLO WORLD!       *\n");
	myPrintf(WHITE, message1);
	
	tskEnd();   //the task is end
}

void myTsk1(void){
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk1: HELLO WORLD!       *\n");
	myPrintf(WHITE, message1);
	
	tskEnd();   //the task is end中文输入
}

void myTsk2(void){
	myPrintf(WHITE, message1);
	myPrintf(WHITE, "*     Tsk2: HELLO WORLD!       *\n");
	myPrintf(WHITE, message1);
	
	tskEnd();  //the task is end
}
