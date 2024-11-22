/*
 * button.h
 *
 *  Created on: Sep 25, 2023
 *      Author: HaHuyen
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "spi.h"
#include "gpio.h"

extern uint16_t button_count[16];

void button_init();
void button_Scan();

uint8_t RollBack();
uint8_t RollNext();
uint8_t Stop();

#endif /* INC_BUTTON_H_ */

