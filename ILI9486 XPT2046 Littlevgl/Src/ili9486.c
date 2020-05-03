#include "ili9486.h"
#include "xpt2046.h"
#include "lv_conf.h"
#include "lvgl/lvgl.h"
#include <string.h>

lv_disp_drv_t * disp;

static void tft_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

void tft_init(void)
{
    begin();
    fillScreen(ILI9486_WHITE);
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf_1[LV_HOR_RES_MAX * 20];
    lv_disp_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * 20);   /*Initialize the display buffer*/

    /*-----------------------------------
    * Register the display in LittlevGL
    *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = 480;
    disp_drv.ver_res = 320;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = tft_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf;

    /*Finally register the driver*/
    disp = lv_disp_drv_register(&disp_drv);
}

static void tft_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > _width - 1) return;
    if(area->y1 > _height - 1) return;

    uint32_t x=0, y=0;

    /*for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            drawPixel(x, y, color_p->full);
            color_p++;
        }
    }*/

    begin_tft_write();
    setWindow(area->x1, area->y1, area->x2, area->y2);
    //pushBlock(ILI9486_BLUE, w*h);

    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //pushBlock(color_p->full, 1);
            tft_Write_16(color_p->full);
            color_p++;
        }
    }
    end_tft_write();

    lv_disp_flush_ready(disp);
    init_XPT2046();
}

void begin_tft_write()
{
    CS_L;
}

void end_tft_write()
{
    CS_H;
}

void writecommand(uint8_t c)
{
  begin_tft_write();

  DC_C;

  tft_Write_8(c);

  DC_D;

  end_tft_write();

}

void writedata(uint8_t d)
{
  begin_tft_write();

  DC_D;        // Play safe, but should already be in data mode

  tft_Write_8(d);
  CS_L;        // Allow more hold time for low VDI rail

  end_tft_write();
}

void begin(void)
{
    _init_width = TFT_WIDTH;
    _init_height = TFT_HEIGHT;

    CS_H;
    DC_D;

    begin_tft_write();

    RST_H;
    HAL_Delay(5);
    RST_L;
    HAL_Delay(20);
    RST_H;

    end_tft_write();

    HAL_Delay(150); // Wait for reset to complete

    begin_tft_write();

    writecommand(0x11); // Sleep out, also SW reset
    HAL_Delay(120);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0xC2);
    writedata(0x44);

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x1F);
    writedata(0x1C);
    writedata(0x0C);
    writedata(0x0F);
    writedata(0x08);
    writedata(0x48);
    writedata(0x98);
    writedata(0x37);
    writedata(0x0A);
    writedata(0x13);
    writedata(0x04);
    writedata(0x11);
    writedata(0x0D);
    writedata(0x00);

    writecommand(0xE1);
    writedata(0x0F);
    writedata(0x32);
    writedata(0x2E);
    writedata(0x0B);
    writedata(0x0D);
    writedata(0x05);
    writedata(0x47);
    writedata(0x75);
    writedata(0x37);
    writedata(0x06);
    writedata(0x10);
    writedata(0x03);
    writedata(0x24);
    writedata(0x20);
    writedata(0x00);

    writecommand(0x20);                     // display inversion OFF

    writecommand(0x36);
    writedata(0x48);

    writecommand(0x29);                     // display on
    HAL_Delay(150);

    end_tft_write();

    begin_tft_write();
    setRotation(1);
    end_tft_write();
}

void setRotation(uint8_t m)
{

  begin_tft_write();

  writecommand(TFT_MADCTL);
    rotation = m % 8;
    switch (rotation) {
     case 0: // Portrait
       writedata(TFT_MAD_BGR | TFT_MAD_MX);
       _width  = _init_width;
       _height = _init_height;
       break;
     case 1: // Landscape (Portrait + 90)
       writedata(TFT_MAD_BGR | TFT_MAD_MV);
       _width  = _init_height;
       _height = _init_width;
       break;
     case 2: // Inverter portrait
       writedata( TFT_MAD_BGR | TFT_MAD_MY);
       _width  = _init_width;
       _height = _init_height;
      break;
     case 3: // Inverted landscape
       writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MX | TFT_MAD_MY);
       _width  = _init_height;
       _height = _init_width;
       break;
     case 4: // Portrait
       writedata(TFT_MAD_BGR | TFT_MAD_MX | TFT_MAD_MY);
       _width  = _init_width;
       _height = _init_height;
       break;
     case 5: // Landscape (Portrait + 90)
       writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MX);
       _width  = _init_height;
       _height = _init_width;
       break;
     case 6: // Inverter portrait
       writedata( TFT_MAD_BGR);
       _width  = _init_width;
       _height = _init_height;
       break;
     case 7: // Inverted landscape
       writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MY);
       _width  = _init_height;
       _height = _init_width;
       break;
    }

  end_tft_write();

  addr_row = 0xFFFF;
  addr_col = 0xFFFF;
}

void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  //begin_tft_write(); // Must be called before setWindow

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;

  // Column addr set
  DC_C; tft_Write_8(TFT_CASET);
  DC_D; tft_Write_32C(x0, x1);

  // Row addr set
  DC_C; tft_Write_8(TFT_PASET);
  DC_D; tft_Write_32C(y0, y1);

  DC_C; tft_Write_8(TFT_RAMWR);

  DC_D;

  //end_tft_write(); // Must be called after setWindow
}

void drawPixel(int32_t x, int32_t y, uint32_t color)
{
  // Range checking
  if ((x < 0) || (y < 0) ||(x >= _width) || (y >= _height)) return;

  begin_tft_write();

  // No need to send x if it has not changed (speeds things up)
  if (addr_col != x) {
    DC_C; tft_Write_8(TFT_CASET);
    DC_D; tft_Write_32D(x);
    addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {
    DC_C; tft_Write_8(TFT_PASET);
    DC_D; tft_Write_32D(y);
    addr_row = y;
  }

  DC_C; tft_Write_8(TFT_RAMWR);
  DC_D; tft_Write_16(color);

  end_tft_write();
}


#define BUF_SIZE 480
void pushBlock(uint16_t color, uint32_t len)
{
  uint16_t col[BUF_SIZE];
  // Always using swapped bytes is a peculiarity of this function...
  uint16_t swapColor = color>>8 | color<<8;
  if  (len<BUF_SIZE) {
    for (uint32_t i = 0; i < len; i++) col[i] = swapColor;
    #ifndef USE_SPI_DMA
    HAL_SPI_Transmit(&SPI_Port, (uint8_t*)col, len<<1, HAL_MAX_DELAY);
    #else
    HAL_SPI_Transmit_DMA(&SPI_Port, (uint8_t*)col, len<<1);
    #endif
    return;
  }

  for (uint32_t i = 0; i < BUF_SIZE; i++) col[i] = swapColor;
  do {
    #ifndef USE_SPI_DMA
    HAL_SPI_Transmit(&SPI_Port, (uint8_t*)col, BUF_SIZE<<1, HAL_MAX_DELAY);
    #else
    HAL_SPI_Transmit_DMA(&SPI_Port, (uint8_t*)col, BUF_SIZE<<1);
    #endif
    len -= BUF_SIZE;
  } while ( len>=BUF_SIZE ) ;
  // Send remaining pixels
    #ifndef USE_SPI_DMA
    if (len) HAL_SPI_Transmit(&SPI_Port, (uint8_t*)col, len<<1, HAL_MAX_DELAY);
    #else
    HAL_SPI_Transmit_DMA(&SPI_Port, (uint8_t*)col, len<<1);
    #endif
}

void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  // Clipping
  if ((x >= _width) || (y >= _height)) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if ((x + w) > _width)  w = _width  - x;
  if ((y + h) > _height) h = _height - y;

  if ((w < 1) || (h < 1)) return;

  begin_tft_write();

  setWindow(x, y, x + w - 1, y + h - 1);

  pushBlock(color, w * h);

  end_tft_write();
}

void fillScreen(uint32_t color)
{
  fillRect(0, 0, _width, _height, color);
}

void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{
  // Clipping
  if ((x < 0) || (x >= _width) || (y >= _height)) return;

  if (y < 0) { h += y; y = 0; }

  if ((y + h) > _height) h = _height - y;

  if (h < 1) return;

  begin_tft_write();

  setWindow(x, y, x, y + h - 1);

  pushBlock(color, h);

  end_tft_write();
}

void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{
  // Clipping
  if ((y < 0) || (x >= _width) || (y >= _height)) return;

  if (x < 0) { w += x; x = 0; }

  if ((x + w) > _width)  w = _width  - x;

  if (w < 1) return;

  begin_tft_write();

  setWindow(x, y, x + w - 1, y);

  pushBlock(color, w);

  end_tft_write();
}
