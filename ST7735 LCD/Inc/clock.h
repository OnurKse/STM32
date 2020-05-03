

#ifndef _CLOCK_H
#define _CLOCK_H

#include <math.h>			//header file for mathematical calculation
#include "st7735.h"
//constants

#define HR_RADIUS 22		//radius of hour hand
#define MIN_RADIUS 38		//radius of minute hand
#define SEC_RADIUS 28		//radius of second hand



//functions

void Draw_clock(uint16_t hr,uint16_t min,uint16_t sec, uint16_t colorHr,uint16_t colorMn,uint16_t colorSc);			//for drawing the total clock at user level
double degree_to_radian(double);						//for degree to radian conversion 
int hr_x_coordinate_finder(uint16_t,uint16_t,double);	//to find the coordinates of hour hand
int hr_y_coordinate_finder(uint16_t,uint16_t,double);	
int min_x_coordinate_finder(uint16_t,uint16_t,double);	//to find the coordinates of min hand
int min_y_coordinate_finder(uint16_t,uint16_t,double);
int sec_x_coordinate_finder(uint16_t,uint16_t,double);	//to find the coordinates of sec hand
int sec_y_coordinate_finder(uint16_t,uint16_t,double);
void Draw_clock_hand(int xcoordinate,int ycoordinate,uint16_t color);						//to draw the hands according to coordinates

#endif
