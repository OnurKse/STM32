/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "demo.h"
#include "main.h"
#if LV_USE_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_style_t style0_button_1;
static lv_style_t style0_label_1;
static void event_handler(lv_obj_t* obj, lv_event_t event);
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void demo_create(void)
{
    lv_obj_t *parent = lv_disp_get_scr_act(NULL);

#ifdef LV_USE_BTN
    lv_style_copy(&style0_button_1, &lv_style_btn_rel);

    lv_obj_t *button_1 = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(button_1, event_handler);
    lv_obj_set_pos(button_1, 163, 115);
    lv_obj_set_size(button_1, 158, 59);
    lv_btn_set_style(button_1, LV_BTN_STYLE_REL, &style0_button_1);
#endif // LV_USE_BTN

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_1, &lv_style_transp);
    style0_label_1.text.color = lv_color_hex(0xffffff);
    style0_label_1.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_1.text.font = &lv_font_roboto_16;
    style0_label_1.text.letter_space = 0;
    style0_label_1.text.line_space = 2;
    style0_label_1.text.opa = 255;

    label_1 = lv_label_create(button_1, NULL);
    lv_label_set_text(label_1, "Text");
    lv_obj_set_pos(label_1, 63, 20);
    lv_obj_set_size(label_1, 32, 19);
    lv_label_set_style(label_1, LV_LABEL_STYLE_MAIN, &style0_label_1);
#endif // LV_USE_LABEL
}

static void event_handler(lv_obj_t* obj, lv_event_t event)
{
    if (event == LV_EVENT_PRESSED) {
        UART_Printf("Pressed\r\n");
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    else if (event == LV_EVENT_RELEASED) {
        UART_Printf("Released\r\n");
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*LV_USE_DEMO*/
