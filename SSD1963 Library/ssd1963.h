//=============================================================================
// SSD1963 driver for STM32 microcontrollers
//=============================================================================
#include "main.h"
//=============================================================================
// All numeric constants with "ULL" suffix !
//=============================================================================
#define TFT_WIDTH				480ULL
#define TFT_HSYNC_BACK_PORCH	2ULL
#define TFT_HSYNC_FRONT_PORCH	0ULL
#define TFT_HSYNC_PULSE			41ULL

#define TFT_HEIGHT				272ULL
#define TFT_VSYNC_BACK_PORCH	2ULL
#define TFT_VSYNC_FRONT_PORCH	0ULL
#define TFT_VSYNC_PULSE			10ULL

#define	TFT_HSYNC_PERIOD	526ULL
#define	TFT_VSYNC_PERIOD	286ULL

#define CS_HIGH TFT_CS_GPIO_Port->BSRR = TFT_CS_Pin
#define CS_LOW  TFT_CS_GPIO_Port->BSRR = (uint32_t)TFT_CS_Pin << 16U

#define WR_HIGH TFT_WR_GPIO_Port->BSRR = TFT_WR_Pin
#define WR_LOW  TFT_WR_GPIO_Port->BSRR = (uint32_t)TFT_WR_Pin << 16U

#define RS_HIGH TFT_RS_GPIO_Port->BSRR = TFT_RS_Pin
#define RS_LOW  TFT_RS_GPIO_Port->BSRR = (uint32_t)TFT_RS_Pin << 16U

#define RD_HIGH TFT_RD_GPIO_Port->BSRR = TFT_RD_Pin
#define RD_LOW  TFT_RD_GPIO_Port->BSRR = (uint32_t)TFT_RD_Pin << 16U

#define RST_HIGH TFT_RST_GPIO_Port->BSRR = TFT_RST_Pin
#define RST_LOW  TFT_RST_GPIO_Port->BSRR = (uint32_t)TFT_RST_Pin << 16U

#define mHIGH(x) (x >> 8)
#define mLOW(x) (x & 0xFF)
//=============================================================================
// Color Defines
//=============================================================================
#define COLOR_BLACK       0x0000      /*   0,   0,   0 */
#define COLOR_NAVY        0x000F      /*   0,   0, 128 */
#define COLOR_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define COLOR_MAROON      0x7800      /* 128,   0,   0 */
#define COLOR_PURPLE      0x780F      /* 128,   0, 128 */
#define COLOR_OLIVE       0x7BE0      /* 128, 128,   0 */
#define COLOR_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define COLOR_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define COLOR_BLUE        0x001F      /*   0,   0, 255 */
#define COLOR_GREEN       0x07E0      /*   0, 255,   0 */
#define COLOR_CYAN        0x07FF      /*   0, 255, 255 */
#define COLOR_RED         0xF800      /* 255,   0,   0 */
#define COLOR_MAGENTA     0xF81F      /* 255,   0, 255 */
#define COLOR_YELLOW      0xFFE0      /* 255, 255,   0 */
#define COLOR_WHITE       0xFFFF      /* 255, 255, 255 */
#define COLOR_ORANGE      0xFD20      /* 255, 165,   0 */
#define COLOR_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define COLOR_PINK        0xF81F
//=============================================================================
// SSD1963 commands
//=============================================================================
#define SSD1963_NOP						0x00
#define SSD1963_SOFT_RESET  			0x01
#define SSD1963_GET_POWER_MODE 			0x0A
#define SSD1963_GET_ADDRESS_MODE		0x0B
#define SSD1963_GET_DISPLAY_MODE		0x0D
#define SSD1963_GET_TEAR_EFFECT_STATUS 	0x0E
#define SSD1963_ENTER_SLEEP_MODE		0x10
#define SSD1963_EXIT_SLEEP_MODE			0x11
#define SSD1963_ENTER_PARTIAL_MODE		0x12
#define SSD1963_ENTER_NORMAL_MODE		0x13
#define SSD1963_EXIT_INVERT_MODE		0x20
#define SSD1963_ENTER_INVERT_MODE		0x21
#define SSD1963_SET_GAMMA_CURVE			0x26
#define SSD1963_SET_DISPLAY_OFF			0x28
#define SSD1963_SET_DISPLAY_ON			0x29
#define SSD1963_SET_COLUMN_ADDRESS		0x2A
#define SSD1963_SET_PAGE_ADDRESS		0x2B
#define SSD1963_WRITE_MEMORY_START		0x2C
#define SSD1963_READ_MEMORY_START		0x2E
#define SSD1963_SET_PARTIAL_AREA		0x30
#define SSD1963_SET_SCROLL_AREA			0x33
#define SSD1963_SET_TEAR_OFF			0x34
#define SSD1963_SET_REAR_ON				0x35
#define SSD1963_SET_ADDRESS_MODE		0x36
#define SSD1963_SET_SCROLL_START		0x37
#define SSD1963_EXIT_IDLE_MODE			0x38
#define SSD1963_ENTER_IDLE_MODE			0x39
#define SSD1963_WRITE_MEMORY_CONTINUE	0x3C
#define SSD1963_READ_MEMORY_CONTINUE	0x3E
#define SSD1963_SET_TEAR_SCANLINE		0x44
#define SSD1963_GET_SCANLINE			0x45
#define SSD1963_READ_DDB				0xA1
#define SSD1963_SET_LCD_MODE			0xB0
#define SSD1963_GET_LCD_MODE			0xB1
#define SSD1963_SET_HORI_PERIOD			0xB4
#define SSD1963_GET_HORI_PERIOD			0xB5
#define SSD1963_SET_VERT_PERIOD			0xB6
#define SSD1963_GET_VERT_PERIOD			0xB7
#define SSD1963_SET_GPIO_CONF			0xB8
#define SSD1963_GET_GPIO_CONF			0xB9
#define SSD1963_SET_GPIO_VALUE			0xBA
#define SSD1963_GET_GPIO_STATUS			0xBB
#define SSD1963_SET_POST_PROC			0xBC
#define SSD1963_GET_POST_PROC			0xBD
#define SSD1963_SET_PWM_CONF			0xBE
#define SSD1963_GET_PWM_CONF			0xBF
#define SSD1963_GET_LCD_GEN0			0xC0
#define SSD1963_SET_LCD_GEN0			0xC1
#define SSD1963_GET_LCD_GEN1			0xC2
#define SSD1963_SET_LCD_GEN1			0xC3
#define SSD1963_GET_LCD_GEN2			0xC4
#define SSD1963_SET_LCD_GEN2			0xC5
#define SSD1963_GET_LCD_GEN3			0xC6
#define SSD1963_SET_LCD_GEN3			0xC7
#define SSD1963_SET_GPIO0_ROP			0xC8
#define SSD1963_GET_GPIO0_ROP			0xC9
#define SSD1963_SET_GPIO1_ROP			0xCA
#define SSD1963_GET_GPIO1_ROP			0xCB
#define SSD1963_SET_GPIO2_ROP			0xCC
#define SSD1963_GET_GPIO2_ROP			0xCD
#define SSD1963_SET_GPIO3_ROP			0xCE
#define SSD1963_GET_GPIO3_ROP			0xCF
#define SSD1963_SET_DBC_CONF			0xD0
#define SSD1963_GET_DBC_CONF			0xD1
#define SSD1963_SET_DBC_TH				0xD4
#define SSD1963_GET_DBC_TH				0xD5
#define SSD1963_SET_PLL					0xE0
#define SSD1963_SET_PLL_MN				0xE2
#define SSD1963_GET_PLL_MN				0xE3
#define SSD1963_GET_PLL_STATUS			0xE4
#define SSD1963_SET_DEEP_SLEEP			0xE5
#define SSD1963_SET_LSHIFT_FREQ			0xE6
#define SSD1963_GET_LSHIFT_FREQ			0xE7
#define SSD1963_SET_PIXEL_DATA_INTERFACE 0xF0
#define SSD1963_PDI_8BIT			0
#define SSD1963_PDI_12BIT			1
#define SSD1963_PDI_16BIT			2
#define SSD1963_PDI_16BIT565		3
#define SSD1963_PDI_18BIT			4
#define SSD1963_PDI_24BIT			5
#define SSD1963_PDI_9BIT			6
#define SSD1963_GET_PIXEL_DATA_INTERFACE 0xF1
//=============================================================================
// Function Prototypes
//=============================================================================
void Send_TFT_Command(char index);
void Send_TFT_Data_8(unsigned char index);
void Send_TFT_Data_16(int index);
void SSD1963_Init(void);
void SSD1963_WindowSet(unsigned int s_x,unsigned int e_x,unsigned int s_y,unsigned int e_y);
void SSD1963_DrawRectagle(unsigned int X1, unsigned int X2, unsigned int Y1, unsigned int Y2, unsigned long color);
void SSD1963_DrawPixel(uint16_t x, uint16_t y, unsigned long color);
void SSD1963_FillScreen(unsigned long color);
