#include "wallClock.h"
int system_ticks;
int HH,MM,SS; // 这里应该定义的是全局变量,setwallCLock应该代表的是给定一个初始化的值,对里面的时钟进行初始化.

void tick(void){
	system_ticks++;
	//你需要填写它
	
	setWallClock(HH,MM,SS);
	return;
}

