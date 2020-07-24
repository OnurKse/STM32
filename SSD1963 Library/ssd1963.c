//=============================================================================
// SSD1963 driver for STM32 microcontrollers
//=============================================================================
#include "stm32f4xx_hal.h"
#include "ssd1963.h"
//=============================================================================
// Write command to SSD1963
//=============================================================================
void Send_TFT_Command(char index)
{
	CS_LOW;
	RS_LOW;
	unsigned int temp;
	temp = GPIOG->ODR;
	temp &= 0xFF00;
	GPIOG->ODR = temp | index;
	WR_LOW;
	asm("NOP");
	WR_HIGH;
	CS_HIGH;
}
//=============================================================================
// Write data to SSD1963
//=============================================================================
void Send_TFT_Data_8(unsigned char index)
{
	CS_LOW;
	RS_HIGH;
	unsigned int temp;
	temp = GPIOG->ODR;
	temp &= 0xFF00;
	GPIOG->ODR = temp | index;
	WR_LOW;
	asm("NOP");
	WR_HIGH;
	CS_HIGH;
}
//=============================================================================
// Write 16-bit data to SSD1963
//=============================================================================
void Send_TFT_Data_16(int index)
{
	CS_LOW;
	RS_HIGH;
	unsigned int temp;
	temp = GPIOE->ODR; // high bits
	temp &= 0x00FF;
	GPIOE->ODR = temp | (index & 0xFF00);
	temp = GPIOG->ODR; // low bits
	temp &= 0xFF00;
	GPIOG->ODR = temp | (index & 0x00FF);
	WR_LOW;
	asm("NOP");
	WR_HIGH;
	CS_HIGH;
}
//=============================================================================
// 
//=============================================================================
void SSD1963_Init(void)
{
	RST_LOW; // Hold in reset
	RS_HIGH; // Enable data access
	CS_HIGH; // Disable LCD
	RD_HIGH;
	WR_HIGH;
	// Release from reset
	HAL_Delay(100);
	RST_HIGH;
	HAL_Delay(100);
	HAL_Delay(100);

	CS_LOW;
	Send_TFT_Command(SSD1963_SOFT_RESET); // Software Reset
	Send_TFT_Command(SSD1963_SOFT_RESET); // Software Reset
	Send_TFT_Command(SSD1963_SOFT_RESET); // Software Reset

	Send_TFT_Command(SSD1963_SET_PLL_MN); // SET PLL freq = 120MHz
	Send_TFT_Data_8(0x23); // M=35, 10x(M+1) = 360Mhz
	Send_TFT_Data_8(0x02); // N=2 360/(N+1) = 120Mhz
	Send_TFT_Data_8(0x54); // Dummy Byte
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_PLL); // PLL settings
	Send_TFT_Data_8(0x01); // START PLL
	HAL_Delay(1);          // Wait to let the PLL stable

	Send_TFT_Command(SSD1963_SET_PLL); // PLL settings
	Send_TFT_Data_8(0x03); // LOCK PLL
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_LCD_MODE); //LCD MODE Settings
	Send_TFT_Data_8(0x28); // TFT panel data width 24bit, FRC, dithering disable
	Send_TFT_Data_8(0x20); // TFT mode
	Send_TFT_Data_8(mHIGH((TFT_WIDTH-1)));  // horizontal panel size(horizontal lines) HightByte
	Send_TFT_Data_8(mLOW((TFT_WIDTH-1)));   // SET horizontal size LowByte
	Send_TFT_Data_8(mHIGH((TFT_HEIGHT-1))); // vertical panel size(vertical lines) HightByte
	Send_TFT_Data_8(mLOW((TFT_HEIGHT-1)));  // SET vertical size LowByte
	Send_TFT_Data_8(0x00); // available if serial RGB mode is selected.
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_PIXEL_DATA_INTERFACE);
	Send_TFT_Data_8(SSD1963_PDI_16BIT565); //SSD1963 data input format data 16 bit, color 565
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_LSHIFT_FREQ); //SET LCD Pixel Clock freq = 9.5MHz = 120MHz * LCDC_FPR / 2^20
	Send_TFT_Data_8(0x01); // highest 4 bits
	Send_TFT_Data_8(0x45); // higher byte
	Send_TFT_Data_8(0x47); // low byte
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_HORI_PERIOD);
	Send_TFT_Data_8(mHIGH((TFT_HSYNC_PERIOD-1))); // horizontal total period (display + non-display)-1 highbyte
	Send_TFT_Data_8(mLOW((TFT_HSYNC_PERIOD-1))); // low byte
	Send_TFT_Data_8(mHIGH((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH))); // Horizontal Pulse Width + Horizontal Back Porch highbyte
	Send_TFT_Data_8(mLOW((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH))); // low byte
	Send_TFT_Data_8(TFT_HSYNC_PULSE-1); // Horizontal Sync Pulse Width
	Send_TFT_Data_8(0x00); // Hsync pulse start position
	Send_TFT_Data_8(0x00); // lowbyte
	Send_TFT_Data_8(0x00); // for serial RGB mode
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_VERT_PERIOD);
	Send_TFT_Data_8(mHIGH((TFT_VSYNC_PERIOD-1))); // vertical total period (display + non-display)-1 highbyte
	Send_TFT_Data_8(mLOW((TFT_VSYNC_PERIOD-1))); // low byte
	Send_TFT_Data_8(mHIGH((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH))); // vertical Pulse Width + vertical Back Porch highbyte
	Send_TFT_Data_8(mLOW((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH))); // low byte
	Send_TFT_Data_8(TFT_VSYNC_PULSE-1); // vertical Sync Pulse Width
	Send_TFT_Data_8(0x00); // Vsync pulse start position
	Send_TFT_Data_8(0x00); // lowbyte
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_ADDRESS_MODE); // Address Mode
	Send_TFT_Data_8(0x00);
	HAL_Delay(1);

	Send_TFT_Command(SSD1963_SET_DISPLAY_ON); //SET display on

	//backlight PWM setting.
	Send_TFT_Command(SSD1963_SET_PWM_CONF);
	Send_TFT_Data_8(0x06); // PLL clock / (256 * (PWMF[7:0])) / 256
	Send_TFT_Data_8(0xFF); // PWM duty cycle -> brightness value
	Send_TFT_Data_8(0x01); // PWM, DBC enable/disable setting.
	Send_TFT_Data_8(0xFF); // DBC manual brightness
	Send_TFT_Data_8(0x00); // DBC minimum brightness
	Send_TFT_Data_8(0x01); // Brightness prescaler
	CS_HIGH;
}
//=============================================================================
//
//=============================================================================
void SSD1963_WindowSet(unsigned int s_x,unsigned int e_x,unsigned int s_y,unsigned int e_y)
{
	Send_TFT_Command(0x2a);        //SET page address
	Send_TFT_Data_8((s_x)>>8);       //SET start page address=0
    Send_TFT_Data_8(s_x&0x00ff);
    Send_TFT_Data_8((e_x)>>8);       //SET end page address=639
    Send_TFT_Data_8(e_x&0x00ff);
    Send_TFT_Command(0x2b);        //SET column address
    Send_TFT_Data_8((s_y)>>8);       //SET start column address=0
    Send_TFT_Data_8(s_y&0x00ff);
    Send_TFT_Data_8((e_y)>>8);       //SET end column address=479
    Send_TFT_Data_8(e_y&0x00ff);
}
//=============================================================================
//
//=============================================================================
void SSD1963_DrawRectagle(unsigned int X1, unsigned int X2, unsigned int Y1, unsigned int Y2, unsigned long color)
{
	unsigned long temp1;
	unsigned long temp2;
	unsigned long frame_pixel;
	unsigned long pixel_CNT;
	temp1= X2-X1+1;
	temp2= Y2-Y1+1;
	frame_pixel=temp1*temp2;
	SSD1963_WindowSet(X1, X2, Y1, Y2);
	Send_TFT_Command(0x2C);
	pixel_CNT=0;
	while(pixel_CNT<=frame_pixel)
	{
		Send_TFT_Data_16(color);
		pixel_CNT++;
	}
}
//=============================================================================
//
//=============================================================================
void SSD1963_DrawPixel(uint16_t x, uint16_t y, unsigned long color)
{
	SSD1963_WindowSet(x, x, y, y);
	Send_TFT_Command(0x2c);
	Send_TFT_Data_16(color);
	Send_TFT_Command(0x0);
}
//=============================================================================
//
//=============================================================================
void SSD1963_FillScreen(unsigned long color)
{
    unsigned int x;
    SSD1963_WindowSet(0, 479, 0, 271);
    Send_TFT_Command(0x2c);
    for (x=0;x<130560;x++) {
    	Send_TFT_Data_16(color);
    }
}
