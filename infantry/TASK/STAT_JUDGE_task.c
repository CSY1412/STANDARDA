/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    STAT_JUDGE_task.c 
  * @brief    ״̬�� 
  * @note     ����һЩ״̬�л�  
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


#include "STAT_JUDGE_task.h"

system_stat_e system_stat;  	//ϵͳ����״̬
chasis_follow_stat_e chasis_follow_stat;  //���̸���״̬
gimabal_main_stat_e gimabal_main_stat; //��̨��״̬
gimabal_aux_mode_e gimabal_aux_mode;  //��ģʽ
control_mode_e control_mode;      //ң�ؼ���ģʽ 
shoot_mode_e shoot_mode;         //���ģʽ
fricmotor_stat_e fricmotor_stat;//Ħ����״̬
u32 last_back_time=0;//����λʱ���¼


/**
  * @brief					����������
  * @author         ��ţ��ţ��
  * @param[in]         
  * @retval       
  */

void STAT_JUDGE_task(void *pvParameters)    //״̬������
{
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��
	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt()))  
			{									
			  RefreshKeyValu();//����һ�ΰ���ֵ
				JudgeSystemStat();//ϵͳ״̬�ж�
				JudgeFricMotor();  //�ж�Ħ����
			  JudgeGimabalStat();  //�ж���̨״̬1			
				JudgeChasisStat();
				KEYFunction();			//���ذ�������
			  JudgeCalibration();//�ж�У׼
			}

	}
}


/**
  * @brief      	ϵͳ״̬�б�  
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void JudgeSystemStat(void)
{		
		if(last_key_valu.s2==1&&this_key_valu.s2==3) //���ϲ�����
		{
			setSystemStat(running);  //ϵͳ����Ϊ����ģʽ
			setGimabalMainStat(control);//��̨��״̬
			setGimabalAuxMode(slowing);
			setChasisFollowStat(self_move);//������
			last_back_time = GetSysTickCnt();  //��ȡϵͳʱ��
			printf("����ģʽ\n");			
		}
		if((slef_check_count.stat&&0x0080!=0)&&this_key_valu.s2==3)  //��������ģʽ���Ҷ�ʧң���ź�
		{
			system_stat=self_stabilization;    //ϵͳ����Ϊ�ȶ�ģʽ
//			printf("�����ȶ�ģʽ\n");
		}
		else if(this_key_valu.s2==1&&system_stat!=breaking)//ң�ؿ���s2���� �ͷ�����
		{		
			system_stat=preparing;  //ϵͳ����Ϊ׼��ģʽ
			gimabal_main_stat=free_and_preparing;
//			printf("����׼��ģʽ\n");			
		}			
		if(system_stat==breaking)//���ϵͳ����
		{
//			printf("���棬ϵͳ���ϣ�����������\n");
		}
		
		if(this_key_valu.s2==3) 
		{
			control_mode=remote;//ң��ģʽ
		}
		else if(this_key_valu.s2==2)
		{	
			control_mode=keyboard;//����ģʽ
		}	
	
		
}

/**
  * @brief      	����״̬�б�  
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void JudgeChasisStat(void)
{
			if(last_key_valu.key_CTRL==DISSPRESS&&this_key_valu.key_CTRL==PRESS)
			{
					chasis_follow_stat=self_move;  //��̨������			
			}
			
			if(last_key_valu.key_CTRL==PRESS&&this_key_valu.key_CTRL==DISSPRESS)
			{
					gimabal_aux_mode=back_posion;  //��̨��λ
					last_back_time = GetSysTickCnt();  //��ȡϵͳʱ��				
			}
		
}
/**
  * @brief      	��̨״̬�б�  
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void JudgeGimabalStat(void)
{	
				if(RC_Ctl.mouse.press_r==PRESS&&manifold.manifold_lock==1)  //�������Ҽ����²�������Ŀ��
			{						
					gimabal_aux_mode=self_aim;  //��������״̬
					chasis_follow_stat=follow_gimabal;  //���̸���
			}
			if(	gimabal_aux_mode==self_aim&&((last_key_valu.key_r==PRESS&&this_key_valu.key_r==DISSPRESS)||manifold.manifold_lock==0))		
			{
					ChangeNowYawAngleToExpangle();
					ChangeNowPitchAngleToExpangle();				
					gimabal_aux_mode=normal;
					chasis_follow_stat=follow_gimabal;  //���̸���
			}
}

void JudgeFricMotor(void)
{
		if(system_stat==running) //��������ģʽ
		{
				ShootControl(); //�������	
		}
		else
		{
			  FRICMOTOR_OFF();//ǿ�ƹر�
				TIM_Cmd(TIM3, DISABLE);  //ʹ��TIM14
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
				
	if(RC_Ctl.rc.s2==3)  //ң��ģʽ
	{
	   if(this_key_valu.key_l==1||RC_Ctl.rc.s1==2)
				{
					TIM_Cmd(TIM3, ENABLE);  //�ر�			
				}
				else
				{
						TIM_Cmd(TIM3, DISABLE);  //�ر�
				}
	}
	if(RC_Ctl.rc.s2==2)  //����ģʽ
	{
				if(shoot_mode==point&&this_key_valu.key_l==1&&last_key_valu.key_l==0)  //����㰴һ��
				{
					shoot.exp_bullet++;  //�����ӵ���Ŀ����
					if(shoot.exp_bullet>10)  //��������ӵ���Ŀ
					{
						shoot.exp_bullet=9;
					}
				}
				
				if(this_key_valu.key_l==1&&shoot_mode==point)  //��һ�ΰ���
				{
						shoot.time_count++;	
						if(shoot.time_count>KEEP_SHOOT_JUDGE_TIME)  //����ʱ�����300ms
						{
								shoot.time_count=0;  //����
								shoot_mode=keep;//��������ģʽ
						}
				}	
				if(shoot_mode==keep&&this_key_valu.key_l==0)   //�˳�����
				{
						shoot_mode=point;//�������ģʽ	
						shoot.exp_bullet=0;  //������Ŀ����
						shoot.time_count=0;						
				}
	}	
		
		
}



void KEYFunction(void)
{
	
		if(this_key_valu.board_s1==1&&last_key_valu.board_s1==0) //����0����
		{	
			 CAN_CMD_CHASSIS_RESET_ID();
		}
		if(this_key_valu.board_s2==1&&last_key_valu.board_s2==0) //��ӡ������Ϣ
		{
					PrintTaskInfomation("STAT_JUDGE_task");
					PrintTaskInfomation("CHASIS_task");
					PrintTaskInfomation("GIMBAL_task");
					PrintTaskInfomation("CONCLUDE_task");
					PrintTaskInfomation("ANO_DT_task");
					PrintTaskInfomation("AUX_task");	
		}
		
			if(this_key_valu.board_s4==1&&last_key_valu.board_s4==0) //��ӡ������Ϣ
		{
				
			  flash_save_data_temp.password_first=FLASH_PASS_WORD;
			  flash_save_data_temp.password_end=FLASH_PASS_WORD;

			  Flash_Data_Save();
		}
		
	
}


void JudgeCalibration(void)
{
			if(system_stat==preparing)  //׼��״̬��  У׼
			{
					static u32 last_record_time=0;
				  static u8 switch_counter=0;
					if((last_key_valu.s1==3&&this_key_valu.s1==2)&&(cal_ch.ch0>1424&&cal_ch.ch1<624))
					{
						if(switch_counter==0)
						{
								last_record_time=GetSysTickCnt(); //��¼��һ��ʱ��
						}
						switch_counter++;					
						if(switch_counter==4) //���ﵽ5��ʱ
						{
								if((GetSysTickCnt()-last_record_time)<3000) // 3����
								{
								    switch_counter=0; //��������
										printf("����һ��У׼P��Y��У׼\n");
										RecordGimbalZeroPosition();
										printf("У׼��ɣ�������\n");
									 
											//����У׼
								}
								else
								{
								 switch_counter=0; //��������
								 printf("У׼��ʱ");
								}						
						}
						if(GetSysTickCnt()-last_record_time>3000&&switch_counter<4)//��ʱ��
						{
							switch_counter=0;	
							printf("У׼��ʱ");
						}				
					}
			}
}



/**
  * @brief      	����ϵͳ״̬
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setSystemStat(system_stat_e enu)
{
	system_stat=enu;
}

/**
  * @brief      	�ж�ϵͳ״̬
  * @author       ��ţ��ţ��
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
  * @brief      	������̨��״̬
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setGimabalMainStat(gimabal_main_stat_e enu)
{
	gimabal_main_stat=enu;
}
/**
  * @brief      	�ж���̨��״̬
  * @author       ��ţ��ţ��
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
  * @brief      	������̨ģʽ
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setGimabalAuxMode(gimabal_aux_mode_e enu)
{
	gimabal_aux_mode=enu;
}
/**
  * @brief      	�ж���̨ģʽ
  * @author       ��ţ��ţ��
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
  * @brief      	���õ��̸���ģʽ
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setChasisFollowStat(chasis_follow_stat_e enu)
{
	chasis_follow_stat=enu;
}
/**
  * @brief      	�жϵ���ģʽ
  * @author       ��ţ��ţ��
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
  * @brief      	���ÿ���ģʽ
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setControlMode(control_mode_e enu)
{
	control_mode=enu;
}
/**
  * @brief      	�жϿ���ģʽ
  * @author       ��ţ��ţ��
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
  * @brief      	�������ģʽ
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setShootMode(shoot_mode_e enu)
{
	shoot_mode=enu;
}
/**
  * @brief      	�ж����ģʽ
  * @author       ��ţ��ţ��
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
  * @brief      	����Ħ����״̬
  * @author       ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void setFricmotorStat(fricmotor_stat_e enu)
{
	fricmotor_stat=enu;
}
/**
  * @brief      	�ж�Ħ����״̬
  * @author       ��ţ��ţ��
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



