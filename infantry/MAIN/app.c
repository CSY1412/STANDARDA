/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file  app.c    
  * @brief ���ܺ���   
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     ��ţ��ţ��     1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  */

#include "app.h"



static u32 sysTickCnt=0;  //ϵͳʱ�����


/**
  * @brief      	�����ʼ��   
  * @author       ��ţ��ţ��
  * @param[in]          
  * @retval       
  */
void BspInit(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	
	delay_init(168);	 //��ʼ����ʱ����
	USART1_Configuration(100000);   //ң����
	USART3_Configuration(460800);
	USART4_Configuration(115200);
	USART5_Configuration(115200);
	USART6_Configuration(500000);  
	CAN1_Configuration();  //CAN1��ʼ��
	LED_Configuration();	 //��ʼ��LED�˿�
	Fricmotor_Configuration();      //Ħ����
	BEEP_Init();					 //��������ʼ��	
	MicroStepMotor_Configuration();  //����
	KEY_Init();
	FlowLED(70);  //��ˮ��ʾ
//	IWDG_Init();
	
	
}

/**
  * @brief      	������ʼ��   
  * @author       ��ţ��ţ��
  * @param[in]          
  * @retval       
  */
void ParamInit(void)
{

	first_order_filter_init(&gesture_sensor_Gx,0.001f,0.02f);   //Gx�����˲���ʼ��
	first_order_filter_init(&chassis_slow_set_vx,0.004f,CHASSIS_ACCEL_X_NUM);    //һ�׵�ͨ�˲�����б����Ϊ�����ٶ�����
	first_order_filter_init(&chassis_slow_set_vy,0.004f,CHASSIS_ACCEL_Y_NUM);    
	first_order_filter_init(&chassis_follow_QE,0.004f,CHASSIS_ACCEL_QE_NUM);    //QEб��
	first_order_filter_init(&chasis_power_control,0.02f,0.2);
	
	USART5_FIFO_Init();//����ϵͳfifo��ʼ��
	FirstFlashRead();//��һ��FLSASH��ȡ
	Rc_Init();//ң����ͨ����ʼ��
	setSystemStat(preparing);  //׼��״̬
	setChasisFollowStat(self_move);  //���̲�����
	confirmGimabalMainStat(free_and_preparing);//��̨�ͷŵ�
	setControlMode(remote);  //����Ϊң��״̬
	setFricmotorStat(off);  //�ر�Ħ����
	BeepNumb(2);
}


/**
  * @brief      	��ӡ������Ϣ   
  * @author       ��ţ��ţ��
  * @param[in]    ���������ַ�      
  * @retval       
  */
void PrintTaskInfomation(char * name)
{
	TaskHandle_t TaskHandle;	
	TaskStatus_t TaskStatus;
	printf("/**********************������Ϣ*************************/\r\n");
	TaskHandle=xTaskGetHandle(name);			//������������ȡ��������
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//������
				 (TaskStatus_t*	)&TaskStatus, 		//������Ϣ�ṹ��
				 (BaseType_t	)pdTRUE,			//����ͳ�������ջ��ʷ��Сʣ���С
			     (eTaskState	)eInvalid);			//�����Լ���ȡ��������׳̬
	//ͨ�����ڴ�ӡ��ָ��������й���Ϣ��
	printf("������:                %s\r\n",TaskStatus.pcTaskName);
	printf("������:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("����׳̬:              %d\r\n",TaskStatus.eCurrentState);
	printf("����ǰ���ȼ�:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("��������ȼ�:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("�����ջ����ַ:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("�����ջ��ʷʣ����Сֵ:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************����***************************/\r\n");
}


/**
  * @brief      	��ȡϵͳʱ��   
  * @author       ��ţ��ţ��
  * @param[in]    ��      
  * @retval       ��ǰϵͳʱ��
  */
u32 GetSysTickCnt(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	/*ϵͳ�Ѿ�����*/
		return xTaskGetTickCount();
	else
		return sysTickCnt;
}

/**
  * @brief       6050�Ƕ���չ(��6050ԭʼ�Ƕȴ�-180��180��չ�������������)  
  * @author      ��ţ��ţ��
  * @param[in]   ����ṹ��   
  * @param[in]   ԭʼ�Ƕ�      
  * @retval      �� 
  */

extend_angle_t extend_angle;  //��չ�Ƕ�
void ExtendAngle(volatile extend_angle_t *v, float raw_angle)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = raw_angle;
	v->diff = v->raw_value - v->last_raw_value;
	if(v->diff < -230)    //���η���ֵ���̫�󣬱�ʾȦ�������˸ı�
	{
		v->round_cnt++;
	}
	else if(v->diff>230)
	{
		v->round_cnt--;
	}		
	v->ecd_value = v->raw_value + v->round_cnt * 360;
}


/**
  * @brief       ��ȡ�ĸ������ֵ  
  * @author      ��ţ��ţ��
  * @param[in]   4����  
  * @param[in]         
  * @retval      ���ֵ 
**/


float GetMax(float a,float b,float c,float d)
{			
		float max;
		 max = a;            //a�ȵ���һ������
		if(b > max)        //���b����������b��Ϊ������������������
				max = b;                        
		if(c > max)         
				max = c;
		if(d > max)
				max = d;
		return max;
}




/**
  * @brief       ���Ź���ʼ��
  * @author      ��ţ��ţ��
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void IWDG_Init(void)
{
	 IWDG->KR = 0x5555;//ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	 IWDG->PR = 0x0002;//����IWDG��Ƶϵ��  
	
	 IWDG->KR = 0x5555;//ʹ�ܶ�IWDG->PR IWDG->RLR��д

	 IWDG->RLR = 0x0FFF;  //����IWDGװ��ֵ  2048ms
	
	 IWDG->KR = 0xCCCC; //ʹ��
	
	 IWDG->KR = 0xAAAA;  //ι��
}


/**
  * @brief       ��Ŀǰpitch��Ƕ���Ϊ����
  * @author      ��ţ��ţ��
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void ChangeNowPitchAngleToExpangle(void)
{
	 pitchdata.pitchexp=(float)(gimbal_motor[1].this_angle-4096); //����ԭ��	
}

/**
  * @brief       ��Ŀǰyaw��Ƕ���Ϊ����
  * @author      ��ţ��ţ��
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void ChangeNowYawAngleToExpangle(void)
{
	 yawdata.yawexp=extend_angle.ecd_value; //����ԭ��	
}




/**
  * @brief       ��¼һ����̨���
  * @author      ��ţ��ţ��
  * @param[in]   
  * @param[in]         
  * @retval      
**/
void RecordGimbalZeroPosition(void)
{
  flash_save_data_temp.gimbal_pitch_zero=gimbal_motor[1].real_angle;
	flash_save_data_temp.gimbal_yaw_zero=gimbal_motor[0].real_angle;
	
	Load_And_Save_Param();  //����

}



/**
  * @brief       ��һ�ζ�ȡPID
  * @author      ��ţ��ţ��
  * @param[in]   
  * @param[in]         
  * @retval      
**/
void FirstFlashRead(void)
{
	if(!Data_Read())  //��ȡflash
	{
		printf("PID���ݳ������������Ӻ�����������\n");
		delay_ms(3000);
  	NVIC_SystemReset();	  //����	
	}
}


