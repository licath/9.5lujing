#include "can.h"

void CAN1_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef 				CAN_InitStructure;
	CAN_FilterInitTypeDef 	CAN_FilterInitStructure;

#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef 				NVIC_InitStructure;
#endif
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);		//打开CAN1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//PA端口时钟打开

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//PA11	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 //上拉输入模式
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		//PA12	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	 //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	


//    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_CAN1);
//    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_CAN1);


//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;//非时间触发通信模式   
	CAN_InitStructure.CAN_ABOM = ENABLE;	//软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM = DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART = ENABLE;//使用报文自动传送 
	CAN_InitStructure.CAN_RFLM = DISABLE;//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode = mode; //模式设置 
	CAN_InitStructure.CAN_SJW = tsjw;//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;//分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);// 初始化CAN1

//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; //16位 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;//16位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//16位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}
