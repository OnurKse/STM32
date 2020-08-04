/*
 * ft5426.h
 *
 *  Created on: Aug 4, 2020
 *      Author: Onur
 */

#ifndef INC_FT5426_H_
#define INC_FT5426_H_

#include "main.h"

#define CTP_I2C_ADDR			0x70

typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint16_t x_coordinate;
	uint16_t y_coordinate;
	uint8_t touch_event;
} CTP;

CTP touch_screen;

void ft5426_write_reg(uint8_t reg, uint8_t value);

void ft5426_read_reg(uint8_t reg, uint8_t *buf, uint8_t len);

void ft5426_init(I2C_HandleTypeDef *hi2c);

void ft5426_readpoint(CTP *temp_data);


#endif /* INC_FT5426_H_ */
