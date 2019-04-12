#ifndef __REFERENCE_TASK_H
#define __REFERENCE_TASK_H
#include "main.h"

#define UP_REG_ID    0xA0  //up layer regional id
#define DN_REG_ID    0xA5  //down layer regional id
#define HEADER_LEN   sizeof(frame_header_t)
#define CMD_LEN      2    //cmdid bytes
#define CRC_LEN      2    //crc16 bytes

#define PROTOCAL_FRAME_MAX_SIZE  200
#define JUDGE_FIFO_BUFLEN 500
typedef enum
{
  STEP_HEADER_SOF  = 0,
  STEP_LENGTH_LOW  = 1,
  STEP_LENGTH_HIGH = 2,
  STEP_FRAME_SEQ   = 3,
  STEP_HEADER_CRC8 = 4,
  STEP_DATA_CRC16  = 5,
} unpack_step_e;

typedef __packed struct
{
  uint8_t  sof;
  uint16_t data_length;
  uint8_t  seq;
  uint8_t  crc8;
} frame_header_t;

typedef struct
{
  FIFO_S_t       *data_fifo;
  frame_header_t *p_header;
  uint16_t       data_len;
  uint8_t        protocol_packet[PROTOCAL_FRAME_MAX_SIZE];
  unpack_step_e  unpack_step;
  uint16_t       index;
} unpack_data_t;

  /* @brief  judgement data command id
  */
typedef enum
{
  GAME_INFO_ID       = 0x0001,  //10Hz
  REAL_BLOOD_DATA_ID = 0x0002,
  REAL_SHOOT_DATA_ID = 0x0003,
	REAL_CHESS_POWER_ID= 0x0004,
  REAL_FIELD_DATA_ID = 0x0005,  //10hZ
  GAME_RESULT_ID     = 0x0006,
  GAIN_BUFF_ID       = 0x0007,
  Robo_Postion_ID			=0X0008,
	
  STU_CUSTOM_DATA_ID = 0x0100,//ÉÏ´«ID

} judge_data_id_e;


/** 
  * @brief  game information structures definition(0x0001)
  *         this package send frequency is 50Hz
  */

typedef __packed struct
{
  uint16_t   stage_remain_time;
  uint8_t    game_process;
  /* current race stage
   0 not start
   1 preparation stage
   2 self-check stage
   3 5 seconds count down
   4 fighting stage
   5 result computing stage */
  uint8_t    robotlevel;
  uint16_t   remain_hp;
  uint16_t   max_hp;  
} game_robot_state_t;

/** 
  * @brief  real time blood volume change data(0x0002)
  */
typedef __packed struct
{
  uint8_t armor_type:4;
 /* 0-3bits: the attacked armor id:
    0x00: 0 front
    0x01:1 left
    0x02:2 behind
    0x03:3 right
    others reserved*/
  uint8_t hurt_type:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01:module offline
    0x02: bullet over speed
    0x03: bullet over frequency */
} robot_hurt_data_t;

/** 
  * @brief  real time shooting data(0x0003)
  */
typedef __packed struct
{
  uint8_t bullet_type;
  uint8_t bullet_freq;
  float   bullet_spd;
} real_shoot_t;
/** 
  * @brief  rfid detect data(0x0004)
  */
typedef __packed struct
{
  float chassisVolt;
	float chassisCurrent;
	float chassisPower;
	float chassisBuffer;
	uint16_t shooterHeat0;
	uint16_t shooterHeat1;
} power_heatdata_t;


/** 
  * @brief  rfid detect data(0x0005)
  */
typedef __packed struct
{
  uint8_t card_type;
  uint8_t card_idx;
} rfid_detect_t;

/** 
  * @brief  game result data(0x0006)
  */
typedef __packed struct
{
  uint8_t winner;
} game_result_t;

/** 
  * @brief  the data of get field buff(0x0007)
  */
typedef __packed struct
{
  uint8_t buff_type;
  uint8_t buff_addition;
} get_buff_t;
/** 
  * @brief  the data of get field buff(0x0008)
  */
typedef __packed struct
{
  float x;
	float y;
	float z;
	float yaw;
} gameRobotPos_t;

/** 
  * @brief  student custom data
  */
typedef __packed struct
{
  float data1;
  float data2;
  float data3;
	uint8_t mask;
} client_show_data_t;


/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  game_robot_state_t game_information;	//0x001
  robot_hurt_data_t  blood_changed_data;//0x002
  real_shoot_t       real_shoot_data;		//0x003
	power_heatdata_t 	 power_heatdata;		//0x004
  rfid_detect_t      rfid_data;					//0x005
  game_result_t      game_result_data;	//0x006
  get_buff_t         get_buff_data;			//0x007
	gameRobotPos_t		 gameRobotPos;			//0x008
 
} receive_judge_t;


extern receive_judge_t judge_rece_mesg;
extern client_show_data_t Client_Show_Data;
extern FIFO_S_t  REFERENCE_FIFO;
uint8_t verify_crc8_check_sum(uint8_t* pchMessage, uint16_t dwLength);
uint8_t verify_crc16_check_sum(uint8_t* pchMessage, uint32_t dwLength);
void unpack_fifo_data(unpack_data_t *p_obj, uint8_t sof);
void append_crc8_check_sum(uint8_t* pchMessage, uint16_t dwLength);
void append_crc16_check_sum(uint8_t* pchMessage, uint32_t dwLength);
void judgement_data_handler(uint8_t *p_frame);
void USART5_FIFO_Init(void);
void REFERENCE_task(void *pvParameters);
#endif