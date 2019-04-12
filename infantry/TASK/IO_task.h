#ifndef _IO_TASK_H
#define _IO_TASK_H
#include "main.h"
//#include <stdint.h>

#define UP_REG_ID       0xA5
#define HEADER_LEN      12
#define CRC16_LEN       2
#define SHOW_DATA_NUM   (40-WAVE_DATA_NUM)
#define PID_DATA_LEN    18
#define WAVE_DATA_NUM   10

#pragma pack(push) //??????
#pragma pack(1)   // 1 bytes??

typedef  enum
{
    INFANTRY_A = 0,	//步兵AAA
    INFANTRY_B = 1,	//步兵BBB
    HERO = 2,//英雄
    ENGINEERING = 3,//工程
    SENTRY = 4,//哨兵
    ARIPLANE = 5//无人机
} CarClass_e; //车型枚举

typedef  enum
{
    WAVE_DATA  = 0,	//波形数据
    SHOW_DATA  = 1, //显示数据
    PID_DATA   = 2,  //PID数据
    PRINTF_DATA   = 3  //printf数据
} CmdID_e;  //功能枚举

typedef  struct
{
    uint8_t     SOF;          //data start Byte,fixed value :0xA5
    uint32_t    CarID;        //car type
    uint32_t    CmdID;        //cmd type
    uint16_t    DataLength;   //data length
    uint8_t     CRC8;         //frame verify
} tFrameHeader; //

typedef  struct
{
    uint16_t P;
    uint16_t I;
    uint16_t D;
} PID_t;


typedef  struct
{
//    tFrameHeader    FrameHeader;//
    float           WaveData[WAVE_DATA_NUM]; //20*10=200 Waveform data
    float           ShowData[SHOW_DATA_NUM];
    PID_t           PID[PID_DATA_LEN];
//    uint16_t        CRC16;         //all data verify ,include CRC16
} BottomToTopData_t; //one frame data receive from bottom device

typedef  struct
{
    tFrameHeader    FrameHeader;//
    PID_t           PID_Data[PID_DATA_LEN];
    uint16_t        CRC16;         //all data verify ,include CRC16
} TopToBottomData_t;    //the PID data,send to bottom device

#pragma pack(pop)//??????


extern u8 Upload_Enable_count;
extern BottomToTopData_t UploadData;
extern uint8_t data_save_flag;
//extern AppParam_t gAppParamStruct;
void AppParamInit(void);
uint8_t AppParamSave(void);
_Bool DataVerify(uint8_t *p);
void data_pack_handle(CarClass_e Car_type,CmdID_e Cmd_id, uint8_t *p_data, uint16_t len);
void TopToBottomDataPPrcess(uint8_t *pData);
void Upload_task(void *pvParameters);
#endif

