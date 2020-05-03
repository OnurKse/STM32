/* vim: set ai et ts=4 sw=4: */
#include "stm32f4xx_hal.h"
#include "st7735.h"

#define DELAY 0x80

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

static void ST7735_Select() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
}

void ST7735_Unselect() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

static void ST7735_Reset() {
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

static void ST7735_ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_Init() {
    ST7735_Select();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

/*
Simpler (and probably slower) implementation:

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color) {
    uint32_t i, b, j;

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                ST7735_DrawPixel(x + j, y + i, color);
            } 
        }
    }
}
*/

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ST7735_Unselect();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&ST7735_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    ST7735_Unselect();
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}

//----------------GFX----------------------

void ST7735_draw_line( unsigned char x1, unsigned char y1,
		unsigned char x2, unsigned char y2,
		uint16_t color)
{
	unsigned char length, xTmp, yTmp, i, y, yAlt;
	int m;

	if(x1 == x2) {									// vertical line
		// x1|y1 must be the upper point
		if(y1 > y2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		length = y2-y1;
		for(i=0; i<=length; i++) {
			ST7735_DrawPixel(x1, y1+i, color);
		}
	} else if(y1 == y2) {							// horizontal line
		// x1|y1 must be the left point
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		length = x2-x1;
		for(i=0; i<=length; i++) {
			ST7735_DrawPixel(x1+i, y1, color);
		}
	} else {
		// x1 must be smaller than x2
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}

		if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) {	// angle larger or equal 45°
			length = x2-x1;								// not really the length :)
			m = ((y2-y1)*200)/length;
			yAlt = y1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+y1;

				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;

				ST7735_draw_line(x1+i, yAlt, x1+i, y, color ); /* wuff wuff recurs. */

				if(length<=(y2-y1) && y1 < y2)
					yAlt = y+1;
				else if(length<=(y1-y2) && y1 > y2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		} else {										// angle smaller 45°
			// y1 must be smaller than y2
			if(y1 > y2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
			length = y2-y1;
			m = ((x2-x1)*200)/length;
			yAlt = x1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+x1;

				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;

				ST7735_draw_line(yAlt, y1+i, y, y1+i, color);
				if(length<=(x2-x1) && x1 < x2)
					yAlt = y+1;
				else if(length<=(x1-x2) && x1 > x2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		}
	}
}



void ST7735_line_h(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t width, uint16_t color)
{
	if(x0>x1)
	{
		uint8_t temp = x0;
		x0 = x1;
		x1 = temp;
	}
	uint8_t i,j;

	if(width%2)
	{
		uint8_t start = y0-(width-1)/2;
		for(i=0;i<width;i++)
			for(j=x0;j<=x1;j++)
				ST7735_DrawPixel(j, start+i, color);
	}
	else
	{
		uint8_t start = y0-width/2;
		for(i=0;i<width;i++)
			for(j=x0;j<=x1;j++)
				ST7735_DrawPixel(j, start+i, color);
	}

}

void ST7735_line_v(uint8_t y0, uint8_t y1, uint8_t x, uint8_t width, uint16_t color)
{
	if(y0>y1)
	{
		uint8_t temp = y0;
		y0 = y1;
		y1 = temp;
	}
	uint8_t i,j;
	if(width%2)
	{
		uint8_t start = x-(width-1)/2;
		for(i=0;i<width;i++)
			for(j=y0;j<y1;j++)
				ST7735_DrawPixel(start+i, j, color);
	}
	else
	{
		uint8_t start = x-width/2;
		for(i=0;i<width;i++)
			for(j=y0;j<y1;j++)
				ST7735_DrawPixel(start+i, j, color);
	}

}


void ST7735_draw_rectangle(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint16_t color)
{
	uint8_t i,j,buffer;
	if(x1>x2)
	{
		buffer = x2;
		x2 = x1;
		x1 = buffer;
	}

	if(y1>y2)
	{
		buffer = y2;
		y2 = y1;
		y1 = buffer;
	}
	for(i=x1;i<=x2;i++)
		for(j=y1;j<y2;j++)
			ST7735_DrawPixel(i, j, color);
}

void ST7735_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  if(r == 0)
    return;
  if (r == 1) {
	  ST7735_DrawPixel(x0, y0, color);
    return;
  }

  int16_t x = -r, y = 0, err = 2-2*r, e2;

  do {
	  ST7735_DrawPixel(x0-x, y0+y, color);
	  ST7735_DrawPixel(x0+x, y0+y, color);
	  ST7735_DrawPixel(x0+x, y0-y, color);
	  ST7735_DrawPixel(x0-x, y0-y, color);

    e2 = err;
    if (e2 <= y) {
      err += ++y*2+1;
      if ((-x == y) && (e2 <= x)) e2 = 0;
    }
    if (e2 > x) err += ++x*2+1;
  } while (x <= 0);
}



//-----------------------------------------

