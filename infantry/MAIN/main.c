/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file
  * @brief
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     3-11-2019       ��ţ��ţ��         1. δ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
 */

#include "main.h"

TaskHandle_t StartTask_Handler;  //������
TaskHandle_t STAT_JUDGETask_Handler;
TaskHandle_t CHASISTask_Handler;
TaskHandle_t GIMBALTask_Handler;
TaskHandle_t CONCLUDETask_Handler;
TaskHandle_t AUXTask_Handler;
TaskHandle_t UploadTask_Handler;
TaskHandle_t CONFIG_PARAM_Task_Handler;
TaskHandle_t REFERENCE_Task_Handler;
TaskHandle_t SHOOT_Task_Handler;


int main(void)
{
    BspInit();  //��ʼ������
    ParamInit();  //������ʼ��
    xTaskCreate(start_task,"start_task",128,NULL,1,&StartTask_Handler);   //������
	  vTaskStartScheduler();          //�����������
    while(1); //���������е���
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
		
    xTaskCreate(STAT_JUDGE_task,"STAT_JUDGE_task",128,NULL,17,&STAT_JUDGETask_Handler);
    xTaskCreate(CHASIS_task,"CHASIS_task",128,NULL,19,&CHASISTask_Handler);
    xTaskCreate(GIMBAL_task,"GIMBAL_task",128,NULL,20,&GIMBALTask_Handler);
    xTaskCreate(CONCLUDE_task,"CONCLUDE_task",128,NULL,21,&CONCLUDETask_Handler);
		xTaskCreate(Upload_task,"Upload_task",512,NULL,17,&UploadTask_Handler); //��λ������
    xTaskCreate(AUX_task,"AUX_task",128,NULL,2,&AUXTask_Handler);
		xTaskCreate(CONFIG_PARAM_Task,"CONFIG_PARAM_Task",128,NULL,5,&CONFIG_PARAM_Task_Handler);		
	  xTaskCreate(REFERENCE_task,"REFERENCE_task",512,NULL,18,&REFERENCE_Task_Handler);	  //����ϵͳ
		xTaskCreate(SHOOT_task,"SHOOT_Task_Handler",128,NULL,4,&SHOOT_Task_Handler);	
	
    printf("Free heap: %d bytes\n\n", xPortGetFreeHeapSize());	/*��ӡʣ���ջ��С*/
    vTaskDelete(StartTask_Handler);    //ɾ����ʼ����

    taskEXIT_CRITICAL();               //�˳��ٽ���
}





/**
  * @brief    ���������Ӻ���
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
  */
void vApplicationIdleHook( void )
{
	static u32 tickWatchdogReset = 0;

	portTickType tickCount = GetSysTickCnt();

	if (tickCount - tickWatchdogReset > WATCHDOG_RESET_MS)
	{
		tickWatchdogReset = tickCount;
		IWDG->KR = 0xAAAA;  //ι��
	}
//	__WFI();	/*����͹���ģʽ*/
}

