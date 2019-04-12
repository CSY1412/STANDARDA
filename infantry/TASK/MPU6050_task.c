#include "main.h"

TaskHandle_t MPU6050Task_Handler;//������


float pitch,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
short temp;			

void MPU6050_task(void *pvParameters)
{

	while(1)
	{
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
			temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������	
		}
				vTaskDelay(1);   //1000hz�ϴ�һ��������������
	}
}
