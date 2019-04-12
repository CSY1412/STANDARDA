#include "main.h"

TaskHandle_t MPU6050Task_Handler;//任务句柄


float pitch,roll,yaw; 		//欧拉角
short aacx,aacy,aacz;		//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
short temp;			

void MPU6050_task(void *pvParameters)
{

	while(1)
	{
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
			temp=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据	
		}
				vTaskDelay(1);   //1000hz上传一次匿名四轴数据
	}
}
