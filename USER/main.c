#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "can.h"
#include "point2point.h"
#include "pid.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "math.h"
#include "move_base.h"
#include "main.h"
#include "ps2.h"
#include "system_stm32f10x.h"

#define PI (3.141593f)
static float laser_distance;
fp32 Px,Py;
float Pp;
fp32 v;
long i;
int a;
u8 SideNumber1,SideNumber2;       //保存车子计划靠的边数 
u8 SideNumber;
u8 flag_cir=0,over_run;                      //判断车子所转的车速
int main(void)
{
	  u8 a=2;
	  u8 b=3;
		u8 state[5]={0};
		u8 switch_state;
		u8 clear_flag=0;
		u8 flag=0,flager;                      //判断车子状态（停车，运动）标志
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    uart_init(115200);//串口初始化为115200
    delay_init();//延时函数初始化
		LCD_Init();
    MyusartInit2(115200);//串口2
    MyusartInit5(115200);
		LED_Init();
		button_init();
    CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_3tq, CAN_BS1_8tq, 3, CAN_Mode_Normal);  //CAN初始化模式,波特率1Mbps
    chassis_init(&chassis_move);//底盘初始化
    delay_ms(50);
    TIM2_Init(3000,36000-1);//1.5秒定时
    TIM3_Init(3000,36000-1);//1.5秒定时
    TIM4_Init(2000,72-1);
    delay_ms(3000);
//		cycleCounterInit();
	  clear();
    while(1)
   {
		    SideNumber1=git_SideNumber1();
		    SideNumber2=git_SideNumber2();
        Px=GetPosX();
        Py=GetPosY();
        Pp=GetAngle();
        if(Px<0)
        {
            LCD_ShowString(60,50,200,16,16,"x=");
            LCD_ShowString(55,70,200,16,16,"-");
            LCD_ShowxNum(60,70,-Px,6,16,0X80);
        }
        else
        {
            LCD_ShowString(60,50,200,16,16,"x=");
            LCD_ShowString(55,70,200,16,16," ");

            LCD_ShowxNum(60,70,Px,6,16,0X80);
        }
        if(Py<0)
        {
            LCD_ShowString(60,90,200,16,16,"y=");
            LCD_ShowString(55,110,200,16,16,"-");
            LCD_ShowxNum(60,110,-Py,6,16,0X80);
        }
        else
        {
            LCD_ShowString(60,90,200,16,16,"y=");
            LCD_ShowString(55,110,200,16,16," ");
            LCD_ShowxNum(60,110,Py,6,16,0X80);
        }
        if(Pp<0)
        {
            LCD_ShowString(60,130,200,16,16,"Angle=");
            LCD_ShowString(55,150,200,16,16,"-");
            LCD_ShowxNum(60,150,-Pp,6,16,0X80);
        }
        else
        {
            LCD_ShowString(60,130,200,16,16,"Angle=");
            LCD_ShowString(55,150,200,16,16," ");
            LCD_ShowxNum(60,150,Pp,6,16,0X80);
        }
				
				    LCD_ShowString(60,170,200,16,16,"sideNumber1=");
            LCD_ShowString(55,190,200,16,16," ");
            LCD_ShowxNum(60,190,SideNumber1,6,16,0X80);
				
				    LCD_ShowString(60,210,200,16,16,"sideNumber2=");
            LCD_ShowString(55,230,200,16,16," ");
            LCD_ShowxNum(60,230,SideNumber2,6,16,0X80);
				
				    LCD_ShowString(130,50,200,16,16,"flag_cir=");
            LCD_ShowString(130,70,200,16,16," ");
            LCD_ShowxNum(130,70,flag_cir,6,16,0X80);
				    
				    LCD_ShowString(130,90,200,16,16,"flag=");
            LCD_ShowString(130,110,200,16,16," ");
            LCD_ShowxNum(130,110,flag,6,16,0X80);
				if(flag_cir%2==0)
					SideNumber=SideNumber1;
				if(flag_cir%2==1)
					SideNumber=SideNumber2;
				if(flag==0)
					{
					clock_wise_close_LargeToSmall(SideNumber);  
					}
				if(flag==1)
					{
//			     clear();
					if(SideNumber==1)
					 UART2_Send_Byte('a');
					if(SideNumber==2)
					 UART2_Send_Byte('b');
					if(SideNumber==3)
					 UART2_Send_Byte('c');
					if(SideNumber==4)
					 UART2_Send_Byte('d');
					 Back_Side(SideNumber);
					}
				if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)==0)&&(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==0))
					{
						LED0=!LED0;
						LED1=!LED0;
						if(flag==0)
						{
						 flag_cir++;
						}
						flag=1;
					}
				if((millis()-i)>5000)
					{
						flag=0;
						a++;
					}
    }
	 
	}
