#ifndef __XPT2046_H__
#define __XPT2046_H__

#include "main.h"
#include "stm32f4xx_hal.h"

#define SPI_Port hspi3
extern SPI_HandleTypeDef hspi3;

#define CS_T_ACTIVE HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET)
#define CS_T_IDLE HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET)



#define Z_THRESHOLD 3500

int16_t xraw, yraw, zraw;
int16_t point_x, point_y;

uint8_t tx_data[1];
uint8_t rx_data[1];

void init_XPT2046(void);
uint16_t Get_Touch(uint8_t addres);
void update(void);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif
