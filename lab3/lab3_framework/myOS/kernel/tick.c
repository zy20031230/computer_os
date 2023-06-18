#include "wallClock.h"
int system_ticks;
int HH,MM,SS; // 这里应该定义的是全局变量,setwallCLock应该代表的是给定一个初始化的值,对里面的时钟进行初始化.

void tick(void){
	int *H_ptr,*M_ptr,*S_ptr;
	H_ptr = &HH;
	M_ptr =&MM;
	S_ptr = &SS;
	getWallClock(H_ptr,M_ptr,S_ptr);//这里应该是拿到当前的时钟,但是还没对于其进行操作
	system_ticks++;
	if(system_ticks==100) {
		system_ticks =0;
		if(SS!=59){
			SS++;
		}
		else {
			SS=0;
			if(MM!=59)
			{
				MM++;
			}
			else{
				MM=0;
				if(HH!=23){
					HH++;
				}
				else{
					HH=0;
				}
			}
		}
	}
	//你需要填写它
	setWallClock(HH,MM,SS);
	return;
}

