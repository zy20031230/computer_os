/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * clear_screen和append2screen必须按照如下借口实现
 * 可以增加其他函数供clear_screen和append2screen调用
 */
extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000

short cur_line=0;
short cur_column=0;//当前光标位置
char * vga_init_p=(char *)0xB8000;


void update_cursor(void){//通过当前行值cur_cline与列值cur_column回写光标
	//填写正确的内容    
}

short get_cursor_position(void){//获得当前光标，计算出cur_line和cur_column的值
	//填写正确的内容    
}


void clear_screen(void) {
	//填写正确的内容    
}

void append2screen(char *str,int color){ 
	//填写正确的内容    
}


