# include<vga.h>
#include<uart.h>
#include<tick.h>
void setWallClock(int HH,int MM,int SS){
	//你需要填写它
	// 这里进行时钟单纯的输出.同时末尾必须输出\n表示换行
	char vga_print[9];
	vga_print[0]='0'+HH/10;
	vga_print[1]='0'+HH%10;
	vga_print[2]=':';
	vga_print[3]='0'+MM/10;
	vga_print[4]='0'+MM%10;
	vga_print[5]=':';
	vga_print[6]='0'+SS/10;
	vga_print[7]='0'+SS%10;
	vga_print[8]='\n';//表示换行
	append2screen(vga_print,0x2f);//用绿色字符输出
	uart_put_chars(vga_print);//头指针导入


	
}

void getWallClock(int *HH_vga,int *MM_vga,int *SS_vga){
	//你需要填写它
	*HH_vga = HH;
	*MM_vga = MM;
	*SS_vga = SS;
}
