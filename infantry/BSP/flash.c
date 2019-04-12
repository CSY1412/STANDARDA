#include "flash.h"

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define PAGE_BYTES	2048
#define PAGE_NUM		64

static u8 clear_flash(u16 sector);
static u8 get_act_page(void);
static u8 write_float(u32 start_addr, u16 cnt, float data);
static u8 write_int(u32 start_addr, u16 cnt, u32 data);
static float read_float(u32 start_addr, u16 cnt);
static u32 read_int(u32 start_addr, u16 cnt);


flash_save_data_t flash_save_data_access;
flash_save_data_t flash_save_data_temp;



void Load_And_Save_Param(void)
{
  		xSemaphoreGive(config_param_xSemaphore);

}

u8 Data_Read(void)
{
		YellowLED(3);
		u8 act_page_num = 255;	//可以使用的页码
    u32 act_page_start = 0;	//该页起始地址
		u8 flash_temp_buff[sizeof(flash_save_data_t)];

    act_page_num = get_act_page();
    if(act_page_num==0)
        return 0;
    else
        act_page_num -= 1;
    act_page_start = FLASH_USER_START_ADDR + (act_page_num * PAGE_BYTES);

		for(u16 data_cnt = 0;data_cnt<sizeof(flash_save_data_t);data_cnt++)
		{
		 flash_temp_buff[data_cnt] = read_byte(act_page_start, data_cnt);		
		}
		 memcpy(&flash_save_data_temp,flash_temp_buff,sizeof(flash_save_data_t));
		
		if(flash_save_data_temp.password_first==FLASH_PASS_WORD&&flash_save_data_temp.password_end==FLASH_PASS_WORD)
		{
		 memcpy(&flash_save_data_access,&flash_save_data_temp,sizeof(flash_save_data_t));
		 printf("FLASH数据校验通过\n");
		}
		else
		{	
			  printf("FLASH数据校验失败,正在重新写入头尾密码\n");
				flash_save_data_temp.password_first=FLASH_PASS_WORD;
			  flash_save_data_temp.password_end=FLASH_PASS_WORD;
			  Flash_Data_Save();
				return 0;			
		}
		

    return 1;
}



u8 Flash_Data_Save(void)
{
		printf("flash写入中！！！！");
	
    u8 act_page_num = 255;	//可以使用的页码
    u32 act_page_start = 0;	//该页起始地址
		u8 flash_temp_buff[sizeof(flash_save_data_t)];

    act_page_num = get_act_page();
    if(act_page_num>(PAGE_NUM-2))
    {
        if(clear_flash(FLASH_Sector_11))
            act_page_num = 0;
        else
            return 0;	//擦除失败
    }

    act_page_start = FLASH_USER_START_ADDR + (act_page_num * PAGE_BYTES);
    FLASH_Unlock();

		memcpy(flash_temp_buff,&flash_save_data_temp,sizeof(flash_save_data_t));

		for(u16 data_cnt = 0;data_cnt<sizeof(flash_save_data_t);data_cnt++)
		{
			if(!write_byte(act_page_start, data_cnt,flash_temp_buff[data_cnt])) return 0;	//写入失败
		}
    FLASH_Lock();   //锁定flash
		YellowLED(5);
		memcpy(&flash_save_data_access,&flash_save_data_temp,sizeof(flash_save_data_t));
		PID_Load();//PID装载
		printf("flash写入完毕！！");
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
static u8 clear_flash(u16 sector)
{
    u8 _return = 0;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    if (FLASH_EraseSector(sector, VoltageRange_3) != FLASH_COMPLETE)
    {
        _return = 0;//失败
    }
    FLASH_Lock();
    _return = 1;	//成功
    return _return;
}
static u8 get_act_page(void)
{
    for(u8 i=0; i<PAGE_NUM; i++)
    {
        if((*(__IO uint32_t*)(FLASH_USER_START_ADDR+i*PAGE_BYTES))==0xffffffff)
        {
            return i;
        }
    }
    return 0xff;
}
static u8 write_float(u32 start_addr, u16 cnt, float data)
{
    if(cnt>510)
        return 0;

    u32 temp;
    temp = *(uint32_t *)(&data);
    if(FLASH_ProgramWord(start_addr+(cnt*4), temp) == FLASH_COMPLETE)
        return 1;
    else
        return 0;	//写入失败
}
static u8 write_int(u32 start_addr, u16 cnt, u32 data)
{
    if(cnt>510)
        return 0;

    if(FLASH_ProgramWord(start_addr+(cnt*4), data) == FLASH_COMPLETE)
        return 1;
    else
        return 0;	//写入失败
}

static u8 write_byte(u32 start_addr, u16 cnt, u8 data)
{
    if(cnt>2000)
        return 0;
    if(FLASH_ProgramByte(start_addr+(cnt), data) == FLASH_COMPLETE)
        return 1;
    else
        return 0;	//写入失败
}
static float read_float(u32 start_addr, u16 cnt)
{
    u32 temp = *(__IO uint32_t*)(start_addr+(cnt*4));
    return *(float *)(&temp);
}
static u32 read_int(u32 start_addr, u16 cnt)
{
    return *(__IO uint32_t*)(start_addr+(cnt*4));
}
static u8 read_byte(u32 start_addr, u16 cnt)
{
    return *(__IO uint8_t*)(start_addr+(cnt));
}






