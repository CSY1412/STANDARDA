#define MPU6050_TASK_PRIO		2 //�������ȼ�
#define MPU6050_STK_SIZE 		128 //�����ջ��С	

extern TaskHandle_t MPU6050Task_Handler;//������


void MPU6050_task(void *pvParameters);