/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    STAT_JUDGE_task.c 
  * @brief    状态机 
  * @note     进行一些状态切换  
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


#include "STAT_JUDGE_task.h"

system_stat_e system_stat;  	//系统运行状态
chasis_follow_stat_e chasis_follow_stat;  //底盘跟随状态
gimabal_main_stat_e gimabal_main_stat; //云台主状态
gimabal_aux_mode_e gimabal_aux_mode;  //子模式
control_mode_e control_mode;      //遥控键盘模式 
shoot_mode_e shoot_mode;         //射击模式
fricmotor_stat_e fricmotor_stat;//摩擦轮状态
u32 last_back_time=0;//做回位时间记录


/**
  * @brief					任务主函数
  * @author         蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */

void STAT_JUDGE_task(void *pvParameters)    //状态机任务
{
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt()))  
			{									
			  RefreshKeyValu();//更新一次按键值
				JudgeSystemStat();//系统状态判断
				JudgeFricMotor();  //判断摩擦轮
			  JudgeGimabalStat();  //判断云台状态1			
				JudgeChasisStat();
				KEYFunction();			//板载按键任务
			  JudgeCalibration();//判断校准
			}

	}
}


/**
  * @brief      	系统状态判别  
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void JudgeSystemStat(void)
{		
		if(last_key_valu.s2==1&&this_key_valu.s2==3) //从上拨到中
		{
			setSystemStat(running);  //系统配置为运行模式
			setGimabalMainStat(control);//云台主状态
			setGimabalAuxMode(slowing);
			setChasisFollowStat(self_move);//不跟随
			last_back_time = GetSysTickCnt();  //获取系统时间
			printf("运行模式\n");			
		}
		if((slef_check_count.stat&&0x0080!=0)&&this_key_valu.s2==3)  //处于运行模式并且丢失遥控信号
		{
			system_stat=self_stabilization;    //系统配置为稳定模式
//			printf("进入稳定模式\n");
		}
		else if(this_key_valu.s2==1&&system_stat!=breaking)//遥控开关s2在上 释放所有
		{		
			system_stat=preparing;  //系统配置为准备模式
			gimabal_main_stat=free_and_preparing;
//			printf("进入准备模式\n");			
		}			
		if(system_stat==breaking)//如果系统故障
		{
//			printf("警告，系统故障！！！！！！\n");
		}
		
		if(this_key_valu.s2==3) 
		{
			control_mode=remote;//遥控模式
		}
		else if(this_key_valu.s2==2)
		{	
			control_mode=keyboard;//键盘模式
		}	
	
		
}

/**
  * @brief      	底盘状态判别  
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void JudgeChasisStat(void)
{
			if(last_key_valu.key_CTRL==DISSPRESS&&this_key_valu.key_CTRL==PRESS)
			{
					chasis_follow_stat=self_move;  //云台不跟随			
			}
			
			if(last_key_valu.key_CTRL==PRESS&&this_key_valu.key_CTRL==DISSPRESS)
			{
					gimabal_aux_mode=back_posion;  //云台回位
					last_back_time = GetSysTickCnt();  //获取系统时间				
			}
		
}
/**
  * @brief      	云台状态判别  
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void JudgeGimabalStat(void)
{	
				if(RC_Ctl.mouse.press_r==PRESS&&manifold.manifold_lock==1)  //如果鼠标右键按下并且锁定目标
			{						
					gimabal_aux_mode=self_aim;  //进入自瞄状态
					chasis_follow_stat=follow_gimabal;  //地盘跟随
			}
			if(	gimabal_aux_mode==self_aim&&((last_key_valu.key_r==PRESS&&this_key_valu.key_r==DISSPRESS)||manifold.manifold_lock==0))		
			{
					ChangeNowYawAngleToExpangle();
					ChangeNowPitchAngleToExpangle();				
					gimabal_aux_mode=normal;
					chasis_follow_stat=follow_gimabal;  //地盘跟随
			}
}

void JudgeFricMotor(void)
{
		if(system_stat==running) //处于正常模式
		{
				ShootControl(); //射击控制	
		}
		else
		{
			  FRICMOTOR_OFF();//强制关闭
				TIM_Cmd(TIM3, DISABLE);  //使能TIM14
		}
		
}


void ShootControl(void)
{
		if(RC_Ctl.rc.s1==3||RC_Ctl.rc.s1==2)
		{
				FRICMOTOR_ON();
				MicroStepMotorControl();
		
			
		}
		else
		{
				FRICMOTOR_OFF();
		}


}



void MicroStepMotorControl(void)
{
				
	if(RC_Ctl.rc.s2==3)  //遥控模式
	{
	   if(this_key_valu.key_l==1||RC_Ctl.rc.s1==2)
				{
					TIM_Cmd(TIM3, ENABLE);  //关闭			
				}
				else
				{
						TIM_Cmd(TIM3, DISABLE);  //关闭
				}
	}
	if(RC_Ctl.rc.s2==2)  //键盘模式
	{
				if(shoot_mode==point&&this_key_valu.key_l==1&&last_key_valu.key_l==0)  //如果点按一次
				{
					shoot.exp_bullet++;  //期望子弹数目增加
					if(shoot.exp_bullet>10)  //限制最大子弹数目
					{
						shoot.exp_bullet=9;
					}
				}
				
				if(this_key_valu.key_l==1&&shoot_mode==point)  //这一次按下
				{
						shoot.time_count++;	
						if(shoot.time_count>KEEP_SHOOT_JUDGE_TIME)  //按下时间大于300ms
						{
								shoot.time_count=0;  //清零
								shoot_mode=keep;//进入连射模式
						}
				}	
				if(shoot_mode==keep&&this_key_valu.key_l==0)   //退出连射
				{
						shoot_mode=point;//进入点射模式	
						shoot.exp_bullet=0;  //点射数目清零
						shoot.time_count=0;						
				}
	}	
		
		
}



void KEYFunction(void)
{
	
		if(this_key_valu.board_s1==1&&last_key_valu.board_s1==0) //按键0按下
		{	
			 CAN_CMD_CHASSIS_RESET_ID();
		}
		if(this_key_valu.board_s2==1&&last_key_valu.board_s2==0) //打印任务信息
		{
					PrintTaskInfomation("STAT_JUDGE_task");
					PrintTaskInfomation("CHASIS_task");
					PrintTaskInfomation("GIMBAL_task");
					PrintTaskInfomation("CONCLUDE_task");
					PrintTaskInfomation("ANO_DT_task");
					PrintTaskInfomation("AUX_task");	
		}
		
			if(this_key_valu.board_s4==1&&last_key_valu.board_s4==0) //打印任务信息
		{
				
			  flash_save_data_temp.password_first=FLASH_PASS_WORD;
			  flash_save_data_temp.password_end=FLASH_PASS_WORD;

			  Flash_Data_Save();
		}
		
	
}


void JudgeCalibration(void)
{
			if(system_stat==preparing)  //准备状态下  校准
			{
					static u32 last_record_time=0;
				  static u8 switch_counter=0;
					if((last_key_valu.s1==3&&this_key_valu.s1==2)&&(cal_ch.ch0>1424&&cal_ch.ch1<624))
					{
						if(switch_counter==0)
						{
								last_record_time=GetSysTickCnt(); //记录第一次时间
						}
						switch_counter++;					
						if(switch_counter==4) //当达到5次时
						{
								if((GetSysTickCnt()-last_record_time)<3000) // 3秒内
								{
								    switch_counter=0; //计数置零
										printf("进行一次校准P轴Y轴校准\n");
										RecordGimbalZeroPosition();
										printf("校准完成！！！！\n");
									 
											//进行校准
								}
								else
								{
								 switch_counter=0; //计数置零
								 printf("校准超时");
								}						
						}
						if(GetSysTickCnt()-last_record_time>3000&&switch_counter<4)//超时间
						{
							switch_counter=0;	
							printf("校准超时");
						}				
					}
			}
}



/**
  * @brief      	设置系统状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setSystemStat(system_stat_e enu)
{
	system_stat=enu;
}

/**
  * @brief      	判断系统状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmSystemStat(system_stat_e enu)
{
	if(system_stat==enu)
		return 1;
	else
		return 0;
}


/**
  * @brief      	设置云台主状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setGimabalMainStat(gimabal_main_stat_e enu)
{
	gimabal_main_stat=enu;
}
/**
  * @brief      	判断云台主状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmGimabalMainStat(gimabal_main_stat_e enu)
{
	if(gimabal_main_stat==enu)
		return 1;
	else
		return 0;
}

/**
  * @brief      	设置云台模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setGimabalAuxMode(gimabal_aux_mode_e enu)
{
	gimabal_aux_mode=enu;
}
/**
  * @brief      	判断云台模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmGimabalAuxMode(gimabal_main_stat_e enu)
{
	if(gimabal_main_stat==enu)
		return 1;
	else
		return 0;
}


/**
  * @brief      	设置底盘跟随模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setChasisFollowStat(chasis_follow_stat_e enu)
{
	chasis_follow_stat=enu;
}
/**
  * @brief      	判断底盘模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmChasisFollowStat(chasis_follow_stat_e enu)
{
	if(chasis_follow_stat==enu)
		return 1;
	else
		return 0;
}


/**
  * @brief      	设置控制模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setControlMode(control_mode_e enu)
{
	control_mode=enu;
}
/**
  * @brief      	判断控制模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmControlMode(control_mode_e enu)
{
	if(control_mode==enu)
		return 1;
	else
		return 0;
}


/**
  * @brief      	设置射击模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setShootMode(shoot_mode_e enu)
{
	shoot_mode=enu;
}
/**
  * @brief      	判断射击模式
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmShootMode(shoot_mode_e enu)
{
	if(shoot_mode==enu)
		return 1;
	else
		return 0;
}

/**
  * @brief      	设置摩擦轮状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void setFricmotorStat(fricmotor_stat_e enu)
{
	fricmotor_stat=enu;
}
/**
  * @brief      	判断摩擦轮状态
  * @author       蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
_Bool confirmFricmotorStat(fricmotor_stat_e enu)
{
	if(fricmotor_stat==enu)
		return 1;
	else
		return 0;
}



