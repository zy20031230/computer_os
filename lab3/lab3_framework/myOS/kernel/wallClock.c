# include<vga.h>
#include<uart.h>
void setWallClock(int HH,int MM,int SS){
	//你需要填写它
	// 这里进行时钟单纯的输出.同时末尾必须输出\n表示换行
	int i;
	char vga_print[9];
	vga_print[0]='0'+HH/10;
	vga_print[1]='0'+HH%10;
	vga_print[2]=':';
	vga_print[3]='0'+MM/10;
	vga_print[4]='0'+MM%10;
	vga_print[5]=':';
	vga_print[6]='0'+SS/10;
	vga_print[7]='0'+SS%10;
	long offest = (80*25-8)*2;
	char *p = (char *)(VGA_BASE+offest);
	for(i=0;i<8;i++){
		*p = vga_print[i];
		*(p+1) = 0x2;
		p=p+2;
	}
}

void getWallClock(int *HH,int *MM,int *SS){
	//你需要填写它
	//HH,MM,SS 从特定的VGA位置得到这个,然后对于全局的HH,MM.SS进行写入.
	long offest = (80*25-8)*2;
	char *p = (char *)(VGA_BASE+offest);
	*HH = (*p - '0')*10+(*(p+2)-'0');
	p=p+6;
	*MM = (*p - '0')*10+(*(p+2)-'0');
	p=p+6;
	*SS = (*p - '0')*10+(*(p+2)-'0');
	if(*HH>23 || *HH<0) *HH = 0;
	if(*MM>59 || *MM<0) *MM =0;
	if(*SS>59 || *SS<0) *SS = 0;
}
