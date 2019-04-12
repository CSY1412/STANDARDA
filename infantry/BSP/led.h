#ifndef __LED_H
#define __LED_H
#include "main.h"
#define LED_BLUE      PEout(5)
#define LED_YELLOW    PEout(6)
#define LED_GREEN     PCout(13)
#define LED_RED       PEout(4)
	
void LED_Configuration(void);
void SetLED(u8 val);  //µÍ4Î»ÓÐÐ§
void LightingGreenLED(void);
void FlowLED(u8 flowspeed);

#endif