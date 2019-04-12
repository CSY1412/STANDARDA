#ifndef __CONFIG_H
#define __CONFIG_H


#define DEBUG_CHASIS 0  
#define DEBUG_GIMBAL 0 


#define KEEP_SHOOT_JUDGE_TIME 230  //判断进入连射模式的延时
#define ONE_BULLET_SHOOT_TIME 230  //一颗子弹的运动延时

#define MAX_CHASIS_POWER 80   //最大底盘功率 单位瓦
#define FLASH_PASS_WORD 0x58  //flash校验密码
#define THIS_CAR_TYPE INFANTRY_A    //这辆车的种类

#define PITCH_MAX 1200.0f // 云台pitch最大    
#define PITCHMIN -1200.0f  // 云台pitch最小   
#define YAW_ROTATE_SPEED_MAX 200  //yaw旋转速度环最大值  
#define PITCHROTATESPEEDMAX 400  //pitch旋转速度环最大值   

#define YAWBACKROTATESPEEDMAX 140  //yaw轴归位速度环最大
#define PITCHBACKROTATESPEEDMAX 210//pitch轴归位速度环最大
#define SLOWLING_BACK_TIME 1400   //慢速回位延时最大

#define YAWQUICKBACKROTATESPEEDMAX 240
#define PITCHQUICKBACKROTATESPEEDMAX 240
#define QUICKBACKTIME 400


#define YAWPROTECTECONDERDETA 330

#define MAXANGLEOFFSET 1500   //云台最大偏角

#define REMOTEPITCHMOVERATE 0.01
#define REMOTEYAWMOVERATE 0.0005

#define MOUSEPITCHMOVERATE 0.12
#define MOUSEYAWMOVERATE 0.004

#define SHIFT_SPEEDMAX 8100
#define MAX_CHASIS_FOLLOW_ECD 1200;  //QE时最大偏角
#define QE_ROATE_SPEED_MAX 3500   //旋转速率最大
#define CHASIS_FOLLOW_ROATE_SPEED_MAX 5500  //底盘跟随时的最大速率
#define WATCHDOG_RESET_MS 100	/*看门狗复位时间*/


#endif