/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file  app.c    
  * @brief 功能函数   
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     蜗牛蜗牛跑     1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  */

#include "app.h"



static u32 sysTickCnt=0;  //系统时间计数


/**
  * @brief      	外设初始化   
  * @author       蜗牛蜗牛跑
  * @param[in]          
  * @retval       
  */
void BspInit(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	
	delay_init(168);	 //初始化延时函数
	USART1_Configuration(100000);   //遥控器
	USART3_Configuration(460800);
	USART4_Configuration(115200);
	USART5_Configuration(115200);
	USART6_Configuration(500000);  
	CAN1_Configuration();  //CAN1初始化
	LED_Configuration();	 //初始化LED端口
	Fricmotor_Configuration();      //摩擦轮
	BEEP_Init();					 //蜂鸣器初始化	
	MicroStepMotor_Configuration();  //方向
	KEY_Init();
	FlowLED(70);  //流水提示
//	IWDG_Init();
	
	
}

/**
  * @brief      	参数初始化   
  * @author       蜗牛蜗牛跑
  * @param[in]          
  * @retval       
  */
void ParamInit(void)
{

	first_order_filter_init(&gesture_sensor_Gx,0.001f,0.02f);   //Gx数据滤波初始化
	first_order_filter_init(&chassis_slow_set_vx,0.004f,CHASSIS_ACCEL_X_NUM);    //一阶低通滤波代替斜波作为底盘速度输入
	first_order_filter_init(&chassis_slow_set_vy,0.004f,CHASSIS_ACCEL_Y_NUM);    
	first_order_filter_init(&chassis_follow_QE,0.004f,CHASSIS_ACCEL_QE_NUM);    //QE斜坡
	first_order_filter_init(&chasis_power_control,0.02f,0.2);
	
	USART5_FIFO_Init();//裁判系统fifo初始化
	FirstFlashRead();//第一次FLSASH读取
	Rc_Init();//遥控器通道初始化
	setSystemStat(preparing);  //准备状态
	setChasisFollowStat(self_move);  //底盘不跟随
	confirmGimabalMainStat(free_and_preparing);//云台释放掉
	setControlMode(remote);  //设置为遥控状态
	setFricmotorStat(off);  //关闭摩擦轮
	BeepNumb(2);
}


/**
  * @brief      	打印任务信息   
  * @author       蜗牛蜗牛跑
  * @param[in]    任务名称字符      
  * @retval       
  */
void PrintTaskInfomation(char * name)
{
	TaskHandle_t TaskHandle;	
	TaskStatus_t TaskStatus;
	printf("/**********************任务信息*************************/\r\n");
	TaskHandle=xTaskGetHandle(name);			//根据任务名获取任务句柄。
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//任务句柄
				 (TaskStatus_t*	)&TaskStatus, 		//任务信息结构体
				 (BaseType_t	)pdTRUE,			//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);			//函数自己获取任务运行壮态
	//通过串口打印出指定任务的有关信息。
	printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
	printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("任务壮态:              %d\r\n",TaskStatus.eCurrentState);
	printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("任务堆栈历史剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************结束***************************/\r\n");
}


/**
  * @brief      	获取系统时间   
  * @author       蜗牛蜗牛跑
  * @param[in]    无      
  * @retval       当前系统时间
  */
u32 GetSysTickCnt(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	/*系统已经运行*/
		return xTaskGetTickCount();
	else
		return sysTickCnt;
}

/**
  * @brief       6050角度扩展(将6050原始角度从-180到180扩展到负无穷到正无穷)  
  * @author      蜗牛蜗牛跑
  * @param[in]   计算结构体   
  * @param[in]   原始角度      
  * @retval      无 
  */

extend_angle_t extend_angle;  //扩展角度
void ExtendAngle(volatile extend_angle_t *v, float raw_angle)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = raw_angle;
	v->diff = v->raw_value - v->last_raw_value;
	if(v->diff < -230)    //两次反馈值差别太大，表示圈数发生了改变
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
  * @brief       获取四个数最大值  
  * @author      蜗牛蜗牛跑
  * @param[in]   4个数  
  * @param[in]         
  * @retval      最大值 
**/


float GetMax(float a,float b,float c,float d)
{			
		float max;
		 max = a;            //a先当第一任擂主
		if(b > max)        //如果b比擂主大，则b成为擂主，否则擂主不变
				max = b;                        
		if(c > max)         
				max = c;
		if(d > max)
				max = d;
		return max;
}




/**
  * @brief       看门狗初始化
  * @author      蜗牛蜗牛跑
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void IWDG_Init(void)
{
	 IWDG->KR = 0x5555;//使能对IWDG->PR IWDG->RLR的写
	
	 IWDG->PR = 0x0002;//设置IWDG分频系数  
	
	 IWDG->KR = 0x5555;//使能对IWDG->PR IWDG->RLR的写

	 IWDG->RLR = 0x0FFF;  //设置IWDG装载值  2048ms
	
	 IWDG->KR = 0xCCCC; //使能
	
	 IWDG->KR = 0xAAAA;  //喂狗
}


/**
  * @brief       将目前pitch轴角度作为期望
  * @author      蜗牛蜗牛跑
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void ChangeNowPitchAngleToExpangle(void)
{
	 pitchdata.pitchexp=(float)(gimbal_motor[1].this_angle-4096); //保持原地	
}

/**
  * @brief       将目前yaw轴角度作为期望
  * @author      蜗牛蜗牛跑
  * @param[in]   
  * @param[in]         
  * @retval      
**/ 
void ChangeNowYawAngleToExpangle(void)
{
	 yawdata.yawexp=extend_angle.ecd_value; //保持原地	
}




/**
  * @brief       记录一次云台零点
  * @author      蜗牛蜗牛跑
  * @param[in]   
  * @param[in]         
  * @retval      
**/
void RecordGimbalZeroPosition(void)
{
  flash_save_data_temp.gimbal_pitch_zero=gimbal_motor[1].real_angle;
	flash_save_data_temp.gimbal_yaw_zero=gimbal_motor[0].real_angle;
	
	Load_And_Save_Param();  //保存

}



/**
  * @brief       第一次读取PID
  * @author      蜗牛蜗牛跑
  * @param[in]   
  * @param[in]         
  * @retval      
**/
void FirstFlashRead(void)
{
	if(!Data_Read())  //读取flash
	{
		printf("PID数据出错！！！三秒钟后重启！！！\n");
		delay_ms(3000);
  	NVIC_SystemReset();	  //重启	
	}
}


