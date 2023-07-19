#ifndef __WALLCLOCK_H__
#define __WALLCLOCK_H__
//通过把所有的时钟相关的文件,均放在这里定义,所以把所有

void getWallClock(int *h, int *m, int *s);
void setWallClock(int h, int m, int s);
void setWallClockHook(void (*func)(void));
#define MAX_HOOK 30
void (*hook_list[MAX_HOOK])(void);
void append_hook(void (*func)(void));
unsigned int second_get();//返回时钟的值
unsigned int kernel_time();//时钟的时间.
#endif