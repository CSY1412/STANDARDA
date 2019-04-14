#include "beep.h"

//��ʼ��PF8Ϊ�����		    
//BEEP IO��ʼ��


void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOFʱ��
  
  //��ʼ����������Ӧ����GPIOF8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_15);  //��������Ӧ����GPIOF8���ͣ� 
}


int beep_valu=0;
/**
  * @brief    ������
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
  */
void BeepAlam(void) 
{
	for(int i=0;i<beep_valu;i++)
	{
		PBout(15)=1;
		vTaskDelay(60);
	}
	PBout(15)=0;//�ر�
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

void BeepNumb(int num)
{
	beep_valu=num*2;
}



