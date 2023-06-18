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
unsigned char * vga_init_p=(unsigned char *)0xB8000;



void update_cursor(void){//通过当前行值cur_cline与列值cur_column回写光标
	//通过其上下8bit数据对齐完成更新,同时可以发现其偏移量为
	unsigned int k_loc;
	k_loc = cur_line * 80 + cur_column;
	unsigned int low,high;
	low = k_loc & 0x000000ff;
	high = (k_loc>>8)&0x000000ff;
	outb(0x3D4,0x0F);
	outb(0x3D5,low);
	outb(0x3D4,0x0E);
	outb(0x3D5,high);
}

short get_cursor_position(void){//获得当前光标，计算出cur_line和cur_column的值
	//填写正确的内容    
	unsigned short low,high;
	unsigned short location;
	outb(0x3D4,0x0f);
	low=inb(0x3d5);
	outb(0x3D4,0x0E);
	high=inb(0x3d5);
	location=low+(high<<8);
	cur_column = location %80;
	cur_line = (location-cur_column)/80;
	
	
}


void clear_screen(void) {
	//填写正确的内容
	unsigned short *clean_srceen = (short *)vga_init_p;
	int i;
	for(i=0;i<25*80*2;i++){
		*clean_srceen = 0x0f20;
		clean_srceen += 1;
	}

	cur_column=0;
	cur_line=0;
	update_cursor();
}
void scroll_func(void){
	unsigned short *srcoll = (short*) vga_init_p;
	int i=0,j=0;
		for(j=0;j<24;j++){
			for(i=0;i<80;i++)//拷贝80个字符
			{
				*(srcoll+80 * j+i) = * (srcoll +80+80*j +i);
			}
		}
		for(i=0;i<80;i++){
			*(srcoll + 80*j +i) = 0x0f20;
		}
		cur_column = 0;
		cur_line = 24;
		update_cursor();
}
//对于color以及字符串指针完成赋值.
void append_word(unsigned char c,int color){

	//通过以上函数,可以通过行列来更新光标,也可以通过函数来获得光标位置.
	//这里采用的是通过行列指标来回写一维偏移量.
	//这里的写法开始是有点错误的,由于这里的字符占位16位,而char的数量是8位,所以会把数字写在错误的位置上
	unsigned char *append_input;
	append_input = vga_init_p + (80 * cur_line +cur_column)*2;
	* append_input = c;
	append_input ++;
	*append_input = color;
	if(cur_column == 79){
		cur_column = 0;
		cur_line++;
	}
	else cur_column = cur_column+1;
	//从这里开始完成,如果下一个输入越界，则完成滚屏操作。
	if(cur_line==25) scroll_func();

	//以上完成正常的光标位置更新
	update_cursor();
}
void append2screen(char *str,int color){ 
	//填写正确的内容    
	int i;
	for(i=0;str[i]!='\0';i++){
		switch (str[i])
		{
		case'\n' :
			if(cur_line==24) scroll_func();
			else{
			cur_column=0;
			cur_line=cur_line+1;
			update_cursor();
			}
			break;	
		case'\r':
			cur_column = 0;
			update_cursor();
			break;
		case'\f':
			clear_screen();
			break;
		case'\b':
			if(cur_column!=0){
				if(cur_line!=0) {cur_line--;update_cursor();}
			}
			break;

		default:
			append_word(str[i],color);
			break;
		}
	}
}



