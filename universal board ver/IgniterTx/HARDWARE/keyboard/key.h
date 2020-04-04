#ifndef __KEY_H
#define __KEY_H

#include "sys.h"


void led_init(void);
void led_running(void);
void led_lock(uint8_t cmd);

void keyboardInit(void);
uint8_t getFireKey(void);
uint8_t getChargeKey(void);
uint8_t getKeyboad(void);


#endif /*__KEY_H*/

