#include "../include/task.h"
#include "../include/wallClock.h"
#include "../include/myPrintk.h"
extern void oneTickUpdateWallClock(void);       //TODO: to be generalized

int hook_num = 0;
void append_hook(void (*func)(void)){
     hook_list[hook_num++] = func;
}
unsigned int tick_number = 0;//这个相当于是时钟频率100HZ的时钟,然后这个值会通过中断进行调用
unsigned int seconds;
void tick(void){
     tick_number++;//作为内核时间在这里出现

     oneTickUpdateWallClock();
     int i = 0;

     if(tick_number  ==0) seconds = 0;//按s作为单位进行计算
     else if(tick_number % 100==0) seconds++;//作为s的初始化单位
     for(i=0;i<hook_num;i++)
     {
          hook_list[i]();
     }



     //tick在调用的时候会对于所有的时钟项目进行处理这就是按100HZ的方式进行处理
     //这里需要补一个挂起的函数式,否则很多函数无法正常的工作.


}
unsigned int second_get(void){ //返回时钟的值
     return seconds;
}
unsigned int kernel_time(){
     return tick_number;//返回当前的时钟的值.
}