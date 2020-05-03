/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "demo.h"
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
static lv_style_t style_screen;
static lv_style_t style0_button_1;
static lv_style_t style0_label_1;
static lv_style_t style0_gauge_1;
static lv_style_t style0_slider_1;
static lv_style_t style0_check_box_1;
/**********************
 *      MACROS
 **********************/
//lv_disp_get_scr_act(NULL)
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void demo_create(void)
{
    lv_obj_t *parent = lv_disp_get_scr_act(NULL);
    lv_style_copy(&style_screen, &lv_style_scr);
        style_screen.body.main_color = lv_color_hex(0xffff00);
        style_screen.body.grad_color = lv_color_hex(0xffff00);
        lv_obj_set_style(parent, &style_screen);

    #ifdef LV_USE_BTN
        lv_style_copy(&style0_button_1, &lv_style_btn_rel);

        lv_obj_t *button_1 = lv_btn_create(parent, NULL);
        lv_obj_set_pos(button_1, 4, 6);
        lv_obj_set_size(button_1, 63, 36);
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

        lv_obj_t *label_1 = lv_label_create(button_1, NULL);
        lv_label_set_text(label_1, "Button");
        lv_obj_set_pos(label_1, 8, 9);
        lv_obj_set_size(label_1, 47, 19);
        lv_label_set_style(label_1, LV_LABEL_STYLE_MAIN, &style0_label_1);
    #endif // LV_USE_LABEL

    #ifdef LV_USE_GAUGE
        lv_style_copy(&style0_gauge_1, &lv_style_pretty_color);
        style0_gauge_1.text.color = lv_color_hex(0x000000);
        style0_gauge_1.text.font = &lv_font_roboto_12;
        style0_gauge_1.text.line_space = 2;

        lv_obj_t *gauge_1 = lv_gauge_create(parent, NULL);
        lv_obj_set_pos(gauge_1, 71, 6);
        lv_obj_set_size(gauge_1, 84, 73);
        lv_gauge_set_style(gauge_1, LV_GAUGE_STYLE_MAIN, &style0_gauge_1);
    #endif // LV_USE_GAUGE

    #ifdef LV_USE_SLIDER
        lv_style_copy(&style0_slider_1, &lv_style_pretty);

        lv_obj_t *slider_1 = lv_slider_create(parent, NULL);
        lv_obj_set_pos(slider_1, 10, 103);
        lv_obj_set_size(slider_1, 89, 22);
        lv_slider_set_value(slider_1, 0, LV_ANIM_OFF);
        lv_slider_set_range(slider_1, 0, 100);
        lv_slider_set_knob_in(slider_1, false);
        lv_slider_set_style(slider_1, LV_SLIDER_STYLE_BG, &style0_slider_1);
    #endif // LV_USE_SLIDER

    #ifdef LV_USE_CB
        lv_style_copy(&style0_check_box_1, &lv_style_transp);

        lv_obj_t *check_box_1 = lv_cb_create(parent, NULL);
        lv_obj_set_pos(check_box_1, 0, 71);
        lv_obj_set_size(check_box_1, 108, 29);
        lv_cb_set_style(check_box_1, LV_CB_STYLE_BG, &style0_check_box_1);
    #endif // LV_USE_CB
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*LV_USE_DEMO*/
