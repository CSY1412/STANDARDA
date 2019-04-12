#include "beep.h"

//初始化PF8为输出口		    
//BEEP IO初始化

int beep_valu=0;

void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOF时钟
  
  //初始化蜂鸣器对应引脚GPIOF8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_15);  //蜂鸣器对应引脚GPIOF8拉低， 
}


void BeepAlam(void) 
{
	for(int i=0;i<beep_valu;i++)
	{
		PBout(15)=1;
		vTaskDelay(60);

	}
	PBout(15)=0;//关闭
	beep_valu=0;	
}

void BeepON(void)
{
		beep_valu=2;
}

void BeepOFF(void)
{
		beep_valu=0;
}





