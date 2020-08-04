/*
 * ft5426.c
 *
 *  Created on: Aug 4, 2020
 *      Author: Onur
 */

#include "ft5426.h"

void ft5426_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    HAL_I2C_Master_Transmit(touch_screen.hi2c, (uint16_t)CTP_I2C_ADDR, buf, 2, HAL_MAX_DELAY);
}

void ft5426_read_reg(uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t data[1];
	data[0] = reg;
	HAL_I2C_Master_Transmit(touch_screen.hi2c, (uint16_t)CTP_I2C_ADDR, data, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(touch_screen.hi2c, (uint16_t)CTP_I2C_ADDR, buf, len, HAL_MAX_DELAY);
}

void ft5426_init(I2C_HandleTypeDef *hi2c)
{
	touch_screen.hi2c = hi2c;
    ft5426_write_reg(0x00, 0);
    ft5426_write_reg(0xA4, 0);
    ft5426_write_reg(0x80, 22);
    ft5426_write_reg(0x88, 12);
}

void ft5426_readpoint(CTP *temp_data)
{
	uint8_t data[4];
	ft5426_read_reg(0x02, data, 1);
	if (data[0] == 1)
	{
		temp_data->touch_event = 1;
		ft5426_read_reg(0x03, data, 4);
		if(((data[0]&0X0F)<<8)+data[1] <= 480 && ((data[2]&0X0F)<<8)+data[3] <= 272)
		{
			temp_data->x_coordinate = ((data[0]&0X0F)<<8)+data[1];
			temp_data->y_coordinate = ((data[2]&0X0F)<<8)+data[3];
		}
	}
	else
		temp_data->touch_event = 0;
}
