#include "xpt2046.h"
#include "lvgl/lvgl.h"

bool XPT2046_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

lv_indev_t * indev;

void touch_init(void)
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = XPT2046_read;
    indev = lv_indev_drv_register(&indev_drv);
}

bool XPT2046_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{

    uint8_t irq = LV_DRV_INDEV_IRQ_READ;
    if (irq == 0) {
        update();
        data->point.x = point_x;
        data->point.y = point_y;
        //UART_Printf("%d %d",point_x ,point_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else
        data->state = LV_INDEV_STATE_REL;
    return false;
}

uint32_t XPT2046_irq_read(void)
{
    return HAL_GPIO_ReadPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin);
}

void init_XPT2046(void)
{
    CS_T_ACTIVE;

    tx_data[0] = 0x90;
    HAL_SPI_Transmit(&SPI_Port, (uint8_t *)tx_data, 1, 100);

    tx_data[0] = 0x00;
    HAL_SPI_Transmit(&SPI_Port, (uint8_t *)tx_data, 1, 100);

    tx_data[0] = 0x00;
    HAL_SPI_Transmit(&SPI_Port, (uint8_t *)tx_data, 1, 100);

    CS_T_IDLE;
}

static int16_t besttwoavg( int16_t x , int16_t y , int16_t z ) {
  int16_t da, db, dc;
  int16_t reta = 0;
  if ( x > y ) da = x - y; else da = y - x;
  if ( x > z ) db = x - z; else db = z - x;
  if ( z > y ) dc = z - y; else dc = y - z;

  if ( da <= db && da <= dc ) reta = (x + y) >> 1;
  else if ( db <= da && db <= dc ) reta = (x + z) >> 1;
  else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

  return (reta);
}

uint16_t Get_Touch(uint8_t addres)
{
    uint16_t touch_x;

    CS_T_ACTIVE;
    tx_data[0] = addres;

    HAL_SPI_Transmit(&SPI_Port, (uint8_t *)tx_data, 1, 1000);

    tx_data[0] = 0x00;
    HAL_SPI_Receive(&SPI_Port, (uint8_t *)rx_data, 1, 100);
    touch_x = rx_data[0];
    touch_x <<= 8;

    HAL_SPI_Receive(&SPI_Port, (uint8_t *)rx_data, 1, 100);
    touch_x |= rx_data[0];
    touch_x >>= 3;
    CS_T_IDLE;

    return touch_x;
}

void update(void)
{
    SPI_Port.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    HAL_SPI_Init(&SPI_Port);
    int16_t data[6];
    HAL_SPI_Transmit(&SPI_Port, (uint8_t *)0xB1, 1, 1000);

    int16_t z1 = Get_Touch(0xC1);
    int z = z1 + 4095;
    int16_t z2 = Get_Touch(0x91);
    z -= z2;
    if (z >= Z_THRESHOLD)
    {
        Get_Touch(0x91);  // dummy X measure, 1st is always noisy
        data[0] = Get_Touch(0xD1);
        data[1] = Get_Touch(0x91); // make 3 x-y measurements
        data[2] = Get_Touch(0xD1);
        data[3] = Get_Touch(0x91);
    }
    else data[0] = data[1] = data[2] = data[3] = 0; // Compiler warns these values may be used unset on early exit.
    data[4] = Get_Touch(0xD0);    // Last Y touch power down
    data[5] = Get_Touch(0);

    if (z < 0) z = 0;
    if (z < Z_THRESHOLD) { //   if ( !touched ) {
        // Serial.println();
        zraw = 0;
        return;
    }
    zraw = z;

    // Average pair with least distance between each measured x then y
    //Serial.printf("    z1=%d,z2=%d  ", z1, z2);
    //Serial.printf("p=%d,  %d,%d  %d,%d  %d,%d", zraw,
        //data[0], data[1], data[2], data[3], data[4], data[5]);
    int16_t x = besttwoavg( data[0], data[2], data[4] );
    int16_t y = besttwoavg( data[1], data[3], data[5] );

    //Serial.printf("    %d,%d", x, y);
    //Serial.println();
    if (z >= Z_THRESHOLD) {
        switch (0) {
          case 0:
            xraw = 4095 - y;
            yraw = x;
            break;
          case 1:
            xraw = x;
            yraw = y;
            break;
          case 2:
            xraw = y;
            yraw = 4095 - x;
            break;
          default: // 3
            xraw = 4095 - x;
            yraw = 4095 - y;
        }
    }
    point_x = map(xraw, 180, 3820, 0, 480);
    point_y = map(yraw, 230, 3850, 0, 320);
    init_XPT2046();
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    HAL_SPI_Init(&hspi3);
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
