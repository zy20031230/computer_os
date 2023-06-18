#include "io.h"
#include "myPrintk.h"
#include "uart.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "wallClock.h"


typedef struct myCommand {
    char name[80];
    char help_content[200];
    int (*func)(int argc, char (*argv)[8]);
}myCommand; 
int func_cmd(int argc, char (*argv)[8]);
int func_help(int argc, char (*argv)[8]);


myCommand cmd={"cmd","List all command\n\0",func_cmd};
myCommand help={"help","Usage: help [command] Display info about [command]\n\0",func_help};

int mystrcmp(const char *str1,const char *str2){
    while(*str1!='\0'&&*str2!='\0'){
        if(*str1 != *str2){
            return(*str1-*str2);
        }
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}//相等返回0

int func_cmd(int argc, char (*argv)[8]){//这个是对应的func_cmd函数对应的操作
    myPrintk(0x7,"List all command\n\0");
    myPrintk(0x07,"cmd\n");
    myPrintk(0x07,"help\n");// 输出所有的命令名.
    return 1;
} 


//这里
int func_help(int argc, char (*argv)[8]){
    // help 调用指定命令的help函数,没有指定函数,则调用help的help函数.
    if (argc==1){//表示只有一个参数,也就是直接调用help的help
        myPrintk(0x7,"Usage: help [command]\nDisplay info about [command]\n\0\n");
        return 0;
    }
    else if(argc>2) {
        myPrintk(0x7,"no such command\n");
        return 1;
    }
    else if(mystrcmp("cmd",argv[1])==0) {//需要去对于指定函数的相关名称完成比较.同时是需要对于argv[1]也就是第二个数组完成相关的比较
        myPrintk(0x7,"List all command\n\0");
        return 0;
    }
    else if(mystrcmp(help.name,argv[1])==0){
        myPrintk(0x7,"Usage: help [command]\nDisplay info about [command]\n");
        return 0;
    }
    else{
        myPrintk(0x7,"no such command\n");
        return 1;        
    }
    
}




// 以下应该是作为主函数去对以上的函数进行调用.
void startShell(void){
//我们通过串口来实现数据的输入
char BUF[256]; //输入缓存区
int BUF_len=0;	//输入缓存区的长度
    int i=0;
    int j=0;
	int argc=0;//用于描述接受到的参数的个数
    char argv[8][8];
    do{
        BUF_len=0; 
        myPrintk(0x7,"Student>>\0");
        while((BUF[BUF_len]=uart_get_char())!='\r'){
            uart_put_char(BUF[BUF_len]);//将串口输入的数存入BUF数组中
            BUF_len++;  //BUF数组的长度加                      
        }
        BUF[BUF_len]='\n';
        append2screen(BUF,0x7);
        BUF[BUF_len]='\r';
        uart_put_chars(" -pseudo_terminal\0");
        uart_put_char('\n');

        //OK,助教已经帮助你们实现了“从串口中读取数据存储到BUF数组中”的任务，接下来你们要做
        //的就是对BUF数组中存储的数据进行处理(也即，从BUF数组中提取相应的argc和argv参
        //数)，再根据argc和argv，寻找相应的myCommand ***实例，进行***.func(argc,argv)函数
        //调用。

        //比如BUF中的内容为 “help cmd”
        //那么此时的argc为2 argv[0]为help argv[1]为cmd
        //接下来就是 help.func(argc, argv)进行函数调用即可

        //按照助教的方式,只需要对于存入buffer的值完成相关比较即可,同时由于定义的mystcmp函数的定义必须是完全一样的,同时需要对于字符串的下一位自动放置0
        while(BUF[i]!='\r'){//注意BUF是以回车的字符结尾的,同时这里采用用空格分割每一个函数名称的方式.
            if(BUF[i]!=' '&&j!=8){
                argv[argc][j]=BUF[i];
                i++;
                j++;
            }//以上正常的读入
            else if(j==8)//表示字符串溢出
            {   
                argc++;
                j = 0;//重置为0
            }
            else if(BUF[i]==' '){//表示在一个读入的过程中遭遇分隔,还是没有输入满8个字符,所以需要完成
                argv[argc][j+1]='\0';//在末尾放置0
                argc++;
                j=0;
                i++;
            }
    }
    if(j!=8){
        argv[argc][j]='\0';
    }//在末尾放置\0字符,同时对于argc++
    argc++;//表示存入了多少个,buf传入了多少参数.
    /*-----------以上完成了相关的argc的初始化---------------*/
    if(mystrcmp("cmd",argv[0])==0){cmd.func(argc,argv);}
    else if(mystrcmp(help.name,argv[0])==0){help.func(argc,argv);}
    else myPrintk(0x7,"Command not found\n");
    /*---------以下完成初始化-----------*/
    for(i=0;i<256;i++)
    BUF[i]='\0';
    i=0;
    j=0;
    argc=0;
    }while(1);

}






/*------------------------- 以下作为存档的原始版本--------------------------*/


// #include "io.h"
// #include "myPrintk.h"
// #include "uart.h"
// #include "vga.h"
// #include "i8253.h"
// #include "i8259A.h"
// #include "tick.h"
// #include "wallClock.h"


// typedef struct myCommand {
//     char name[80];
//     char help_content[200];
//     int (*func)(int argc, char (*argv)[8]);
// }myCommand; 
// int func_cmd(int argc, char (*argv)[8]);
// int func_help(int argc, char (*argv)[8]);
// myCommand cmd={"cmd\0","List all command\n\0",func_cmd};
// myCommand help={"help\0","Usage: help [command]\n\0Display info about [command]\n\0",func_help};

// int mystrcmp(const char *str1,const char *str2){
//     while(*str1!='\0'&&*str2!='\0'){
//         if(*str1 != *str2){
//             return(*str1-*str2);
//         }
//         str1++;
//         str2++;
//     }
//     return (*str1 - *str2);
// }//相等返回0

// int func_cmd(int argc, char (*argv)[8]){//这个是对应的func_cmd函数对应的操作
//     myPrintk(0x07,"%s\n",cmd.name);
//     myPrintk(0x07,"%s\n",help.name);// 输出所有的命令名.
//     return 1;
	
// } 


// //这里
// int func_help(int argc, char (*argv)[8]){
//     // help 调用指定命令的help函数,没有指定函数,则调用help的help函数.
//     if (argc==1){//表示只有一个参数,也就是直接调用help的help
//         myPrintk(0x7,"Usage: help [command]\n\0Display info about [command]\n\0\n",help.help_content);
//         return 0;
//     }
//     else if(argc>2) {
//         myPrintk(0x7,"no such command\n");
//         return 1;
//     }
//     else if(mystrcmp(cmd.name,argv[1])==0) {//需要去对于指定函数的相关名称完成比较.同时是需要对于argv[1]也就是第二个数组完成相关的比较
//         myPrintk(0x7,"List all command\n\0");
//         return 0;
//     }
//     else if(mystrcmp(help.name,argv[1])==0){
//         myPrintk(0x7,"%s\n",help.help_content);
//         return 0;
//     }
//     else{
//         myPrintk(0x7,"no such command\n");
//         return 1;        
//     }
    
// }




// // 以下应该是作为主函数去对以上的函数进行调用.
// void startShell(void){
// //我们通过串口来实现数据的输入
// char BUF[256]; //输入缓存区
// int BUF_len=0;	//输入缓存区的长度
//     int i=0;
//     int j=0;
// 	int argc=0;//用于描述接受到的参数的个数
//     char argv[8][8];
//     do{
//         BUF_len=0; 
//         myPrintk(0x7,"Student>>\0");
//         while((BUF[BUF_len]=uart_get_char())!='\r'){
//             uart_put_char(BUF[BUF_len]);//将串口输入的数存入BUF数组中
//             BUF_len++;  //BUF数组的长度加                      
//         }
//         uart_put_chars(" -pseudo_terminal\0");
//         uart_put_char('\n');

//         //OK,助教已经帮助你们实现了“从串口中读取数据存储到BUF数组中”的任务，接下来你们要做
//         //的就是对BUF数组中存储的数据进行处理(也即，从BUF数组中提取相应的argc和argv参
//         //数)，再根据argc和argv，寻找相应的myCommand ***实例，进行***.func(argc,argv)函数
//         //调用。

//         //比如BUF中的内容为 “help cmd”
//         //那么此时的argc为2 argv[0]为help argv[1]为cmd
//         //接下来就是 help.func(argc, argv)进行函数调用即可

//         //按照助教的方式,只需要对于存入buffer的值完成相关比较即可,同时由于定义的mystcmp函数的定义必须是完全一样的,同时需要对于字符串的下一位自动放置0
//         while(BUF[i]!='\r'){//注意BUF是以回车的字符结尾的,同时这里采用用空格分割每一个函数名称的方式.
//             if(BUF[i]!=' '&&j!=8){
//                 argv[argc][j]=BUF[i];
//                 i++;
//                 j++;
//             }//以上正常的读入
//             else if(j==8)//表示字符串溢出
//             {
//                 argc++;
//                 j = 0;//重置为0
//             }
//             else if(BUF[i]==' '){//表示在一个读入的过程中遭遇分隔,还是没有输入满8个字符,所以需要完成
//                 argv[argc][j+1]='\0';//在末尾放置0
//                 argc++;
//                 j=0;
//             }
//     }
//     if(j!=8){
//         argv[argc][j+1]='\0';
//     }//在末尾放置\0字符,同时对于argc++
//     argc++;//表示存入了多少个,buf传入了多少参数.
//     /*-----------以上完成了相关的argc的初始化---------------*/
//     if(mystrcmp(cmd.name,argv[0])){cmd.func(argc,argv);}
//     else if(mystrcmp(help.name,argv[0])){help.func(argc,argv);}
//     else myPrintk(0x7,"Command not found\n");
//     /*----以下完成初始化-----------*/
//     i=0;
//     j=0;
//     argc=0;
//     }while(1);

// }


