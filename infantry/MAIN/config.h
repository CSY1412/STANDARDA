#ifndef __CONFIG_H
#define __CONFIG_H








#define MAX_CHASIS_FOLLOW_ECD 1200;  //QE时最大偏角
#define QE_ROATE_SPEED_MAX 3500   //QE旋转速率最大


/*------------------------------------射击配置--------------------------------*/
#define KEEP_SHOOT_JUDGE_TIME 230  //判断进入连射模式的延时
#define ONE_BULLET_SHOOT_TIME 230  //一颗子弹的运动延时

/*------------------------------------遥控器配置--------------------------------*/
#define RC_DEAD_BAND 50  //遥控器死区值
/*------------------------------------云台配置--------------------------------*/

#define MAX_YAW_OFFSET_ANGLE 60   //云台yaw最大偏角
#define PITCH_MAX 20.0f // 云台pitch最大    
#define PITCH_MIN -20.0f  // 云台pitch最小   
#define YAW_ROTATE_SPEED_MAX 700  //yaw旋转速度环最大值  

#define GIMBAL_YAW_BACK_KP     70    //回位yaw位置环
#define GIMBAL_PITCH_BACK_KP    1   //回位pitch位置换
#define PITCH_ROTATE_SPEED_MAX 400  //pitch旋转速度环最大值   
#define YAW_BACK_ROTATE_SPEED_MAX 350  //yaw轴归位速度环最大
#define PITCH_BACK_ROTATE_SPEED_MAX 210//pitch轴归位速度环最大
#define SLOWLING_BACK_TIME 800   //回位延时最大

#define YAW_PROTECTE_OFFSET_ANGLE 10  //偏移度
#define REMOTE_PITCH_MOVE_SPEEDRATE 0.01  //遥控器P轴控制速率
#define REMOTE_YAW_MOVE_SPEEDRATE 0.0005    //遥控器Y轴控制速率
#define MOUSE_PITCH_MOVE_SPEEDRATE 0.12
#define MOUSE_YAW_MOVE_SPEEDRATE 0.004
#define MAX_M6623_CURRENT 5000
/*------------------------------------系统配置--------------------------------*/
#define DEBUG_CHASIS 0  
#define DEBUG_GIMBAL 0 
#define WATCHDOG_RESET_MS 100  //看门狗重启时间(ms)
#define FLASH_PASS_WORD 0x58  //flash校验密码
#define THIS_CAR_TYPE INFANTRY_A    //这辆车的种类


/*------------------------------------底盘配置--------------------------------*/
#define MAX_CHASIS_POWER 80   //最大底盘功率 单位瓦


#define CHASSIS_ACCEL_X_NUM 0.0966666667f //一阶低通滤波系数
#define CHASSIS_ACCEL_Y_NUM 0.03f   //0.07
#define CHASSIS_ACCEL_QE_NUM 0.15f

#define KEY_CHASSIS_SPEED_X 1.5f  //按键底盘运动过程最大平移速度
#define KEY_CHASSIS_SPEED_Y 2.2f  //按键底盘运动过程最大前进速度
#define RC_CHASSIS_SPEED_X 1.5f  //遥控器底盘运动过程最大平移速度
#define RC_CHASSIS_SPEED_Y 2.0f  //遥控器底盘运动过程最大前进速度
#define SHIFT_CHASSIS_SPEED_Y 1.0f*MAX_M3508_SPEED  //底盘运动过程最大前进速度
#define CHASIS_FOLLOW_ROATE_SPEED_MAX 11  //底盘跟随时的最大旋转速率




/*------------------------------------底盘电机--------------------------------*/
#define M3508_MOTOR_RPM_TO_VECTOR 0.000415809748903494517209f //m3508转化成底盘速度(m/s)的比例，
#define MAX_M3508_RPM 8100   //最大3508电机转速
#define MAX_M3508_CURRENT 16384   //最大3508电机电流
#define MAX_M3508_SPEED M3508_MOTOR_RPM_TO_VECTOR*MAX_M3508_RPM //最大轮子线速度
#define CHASSIS_ROATE_SET_SCALE 0.1f  //底盘设置旋转速度，设置前后左右轮不同设定速度的比例分权 0为在几何中心，不需要补偿
#define MOTOR_DISTANCE_TO_CENTER 0.1f  //底盘距离中心的距离
#define MAX_WHEEL_SPEED 4.0f         //底盘电机最大速度

#endif