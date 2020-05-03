#ifndef __ILI9486_H__
#define __ILI9486_H__

#include "main.h"
#include "stm32f4xx_hal.h"

#define TFT_WIDTH  320
#define TFT_HEIGHT 480

#define ILI9486_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9486_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9486_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9486_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9486_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9486_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9486_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9486_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9486_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9486_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9486_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9486_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9486_RED         0xF800      /* 255,   0,   0 */
#define ILI9486_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9486_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9486_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9486_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9486_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9486_PINK        0xF81F


// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C

#define TFT_RAMRD   0x2E
#define TFT_IDXRD   0x00 // ILI9341 only, indexed control register read

#define TFT_MADCTL  0x36
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_SS  0x02
#define TFT_MAD_GS  0x01
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21

#define DC_D         TFT_DC_GPIO_Port->BSRR = TFT_DC_Pin
#define DC_C         TFT_DC_GPIO_Port->BSRR = (uint32_t)TFT_DC_Pin << 16U
#define CS_H         TFT_CS_GPIO_Port->BSRR = TFT_CS_Pin
#define CS_L         TFT_CS_GPIO_Port->BSRR = (uint32_t)TFT_CS_Pin << 16U
#define RST_H        TFT_RST_GPIO_Port->BSRR = TFT_RST_Pin
#define RST_L        TFT_RST_GPIO_Port->BSRR = (uint32_t)TFT_RST_Pin << 16U

#define swap(a, b) { int16_t t = a; a = b; b = t; }

//#define USE_SPI_DMA

#ifndef USE_SPI_DMA
#define tft_Write_8(C) \
{ spiBuffer[0] = 0; spiBuffer[1] = C; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 2, 10); }

#define tft_Write_16(C) \
{ spiBuffer[0] = (C)>>8; spiBuffer[1] = C; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 2, 10); }

#define tft_Write_16S(C) \
{ spiBuffer[0] = C; spiBuffer[1] = (C)>>8; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 2, 10); }

#define tft_Write_32(C) \
{ spiBuffer[1] = ((C)>>24); spiBuffer[3] = ((C)>>16); spiBuffer[5] = ((C)>>8); spiBuffer[7] = C; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 8, 10); }

#define tft_Write_32C(C,D) \
{ spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((D)>>8); spiBuffer[7] = D; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 8, 10); }

#define tft_Write_32D(C) \
{ spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((C)>>8); spiBuffer[7] = C; \
HAL_SPI_Transmit(&SPI_Port, spiBuffer, 8, 10); }
#else
#define tft_Write_8(C) \
{ spiBuffer[0] = 0; spiBuffer[1] = C; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }

#define tft_Write_16(C) \
{ spiBuffer[0] = (C)>>8; spiBuffer[1] = C; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }

#define tft_Write_16S(C) \
{ spiBuffer[0] = C; spiBuffer[1] = (C)>>8; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }

#define tft_Write_32(C) \
{ spiBuffer[1] = ((C)>>24); spiBuffer[3] = ((C)>>16); spiBuffer[5] = ((C)>>8); spiBuffer[7] = C; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }

#define tft_Write_32C(C,D) \
{ spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((D)>>8); spiBuffer[7] = D; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }

#define tft_Write_32D(C) \
{ spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((C)>>8); spiBuffer[7] = C; \
HAL_SPI_Transmit_DMA(&SPI_Port, spiBuffer, 8); }
#endif

#define SPI_Port hspi3
extern SPI_HandleTypeDef hspi3;

uint8_t   spiBuffer[8];
int32_t  addr_row, addr_col;
uint8_t  rotation;
int32_t  _width, _height;
int32_t _init_width;
int32_t _init_height;

void tft_init(void);
void begin_tft_write();
void end_tft_write();
void writecommand(uint8_t c);
void writedata(uint8_t d);
void begin(void);
void setRotation(uint8_t m);
void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void pushBlock(uint16_t color, uint32_t len);
void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void fillScreen(uint32_t color);
void drawPixel(int32_t x, int32_t y, uint32_t color);
void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);

#endif
