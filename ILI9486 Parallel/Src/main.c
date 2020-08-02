/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint8_t  dir;
    uint16_t  wramcmd;
    uint16_t  rramcmd;
    uint16_t  setxcmd;
    uint16_t  setycmd;
}_lcd_dev;

_lcd_dev lcddev;

#define LCD_W 320
#define LCD_H 480

#define LCD_CS_SET  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
#define LCD_RS_SET  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
#define LCD_RST_SET HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
#define LCD_WR_SET  HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
#define LCD_RD_SET  HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_SET);

#define LCD_CS_CLR  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
#define LCD_RS_CLR  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
#define LCD_RST_CLR HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
#define LCD_WR_CLR  HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
#define LCD_RD_CLR  HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_RESET);

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN           0XBC40
#define BRRED           0XFC07
#define GRAY            0X8430

#define DARKBLUE         0X01CF
#define LIGHTBLUE        0X7D7C
#define GRAYBLUE         0X5458

#define LIGHTGREEN      0X841F
#define LIGHTGRAY     0XEF5B
#define LGRAY                   0XC618

#define LGRAYBLUE       0XA651
#define LBBLUE          0X2B12

void LCD_Init(void);
void LCD_write(uint16_t VAL);
void LCD_Clear(uint16_t Color);
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void LCD_WR_REG(uint8_t data);
void LCD_WR_DATA(uint8_t data);
void LCD_WriteRAM_Prepare(void);
void Lcd_WriteData_16Bit(uint16_t Data);
void LCD_direction(uint8_t direction );


void LCD_write(uint16_t VAL)
{
    LCD_CS_CLR;
    HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (VAL & 0x01));
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, (VAL & 0x02)>>1);
    HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (VAL & 0x04)>>2);
    HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (VAL & 0x08)>>3);
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (VAL & 0x10)>>4);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (VAL & 0x20)>>5);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (VAL & 0x40)>>6);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (VAL & 0x80)>>7);

    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}

void LCD_WR_REG(uint8_t data)
{
   LCD_RS_CLR;
   LCD_write(data);
}

void LCD_WR_DATA(uint8_t data)
{
     LCD_RS_SET;
     LCD_write(data);
}

void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

void Lcd_WriteData_16Bit(uint16_t Data)
{
    LCD_RS_SET;
    LCD_write(Data >> 8);
    LCD_write(Data & 0xFF);
}

void LCD_Clear(uint16_t Color)
{
    unsigned int i;//,m;
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
    for(i=0;i<lcddev.height*lcddev.width;i++)
    {
        Lcd_WriteData_16Bit(Color);
    }
}

void LCD_RESET(void)
{
    LCD_RST_CLR;
    HAL_Delay(10);
    LCD_RST_SET;
    HAL_Delay(5);
}

void LCD_Init(void)
{
    LCD_RST_SET;
    LCD_RD_SET;
    LCD_CS_SET;
    LCD_WR_SET;
    LCD_CS_SET;

    LCD_RESET();
    LCD_WR_REG(0XF1);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0X0F);
    LCD_WR_DATA(0x8F);
    LCD_WR_REG(0XF2);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0xA3);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0XB2);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0XF8);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x04);
    LCD_WR_REG(0XF9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x41);
    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x91);
    LCD_WR_DATA(0x80);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0x98);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x2E);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x47);
    LCD_WR_DATA(0x75);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);
    LCD_WR_REG(0x11);
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x28);
    HAL_Delay(120);
    LCD_WR_REG(0x29);

    LCD_direction(1);
    //LCD_Clear(WHITE);
}

void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar>>8);
    LCD_WR_DATA(0x00FF&xStar);
    LCD_WR_DATA(xEnd>>8);
    LCD_WR_DATA(0x00FF&xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar>>8);
    LCD_WR_DATA(0x00FF&yStar);
    LCD_WR_DATA(yEnd>>8);
    LCD_WR_DATA(0x00FF&yEnd);

    LCD_WriteRAM_Prepare();
}

void LCD_direction(uint8_t direction)
{
    lcddev.setxcmd=0x2A;
    lcddev.setycmd=0x2B;
    lcddev.wramcmd=0x2C;
    lcddev.rramcmd=0x2E;
    switch(direction){
        case 0:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;
            LCD_WriteReg(0x36,(1<<6)|(1<<3));//0 degree MY=0,MX=0,MV=0,ML=0,BGR=1,MH=0
        break;
        case 1:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(1<<4)|(1<<5));//90 degree MY=0,MX=1,MV=1,ML=1,BGR=1,MH=0
        break;
        case 2:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;
            LCD_WriteReg(0x36,(1<<3)|(1<<7));//180 degree MY=1,MX=1,MV=0,ML=0,BGR=1,MH=0
        break;
        case 3:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(1<<5)|(1<<6)|(1<<7));//270 degree MY=1,MX=0,MV=1,ML=0,BGR=1,MH=0
        break;
        default:break;
    }
}

void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i,j;
    uint16_t width=ex-sx+1;
    uint16_t height=ey-sy+1;
    LCD_SetWindows(sx,sy,ex,ey);
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        Lcd_WriteData_16Bit(color); //写入数�?�
    }
   // LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();
  LCD_Clear(RED);
  LCD_Fill(50,50,200,200, BLUE);
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin|LCD_D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LD2_Pin 
                          |LCD_D7_Pin|LCD_D0_Pin|LCD_D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin 
                          |LCD_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RST_Pin LCD_D1_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LD2_Pin 
                           LCD_D7_Pin LCD_D0_Pin LCD_D2_Pin */
  GPIO_InitStruct.Pin = LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LD2_Pin 
                          |LCD_D7_Pin|LCD_D0_Pin|LCD_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin 
                           LCD_D4_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin 
                          |LCD_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
