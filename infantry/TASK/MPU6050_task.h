#define MPU6050_TASK_PRIO		2 //任务优先级
#define MPU6050_STK_SIZE 		128 //任务堆栈大小	

extern TaskHandle_t MPU6050Task_Handler;//任务句柄


void MPU6050_task(void *pvParameters);