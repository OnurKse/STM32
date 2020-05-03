#include "app.h"

/**********************
 *       WIDGETS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

void app_create(void)
{
    lv_obj_t *parent = lv_disp_get_scr_act(NULL);

#ifdef LV_USE_CONT
    lv_style_copy(&style0_container_2, &lv_style_pretty);
    style0_container_2.body.main_color = lv_color_hex(0x00cb00);
    style0_container_2.body.grad_color = lv_color_hex(0x00cb00);
    style0_container_2.body.radius = 0;
    style0_container_2.body.border.width = 0;

    container_2 = lv_cont_create(parent, NULL);
    lv_obj_set_pos(container_2, 0, 26);
    lv_obj_set_size(container_2, 160, 26);
    lv_cont_set_style(container_2, LV_CONT_STYLE_MAIN, &style0_container_2);
#endif // LV_USE_CONT

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_1, &lv_style_transp);
    style0_label_1.text.color = lv_color_hex(0xffffff);
    style0_label_1.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_1.text.font = &lv_font_roboto_16;
    style0_label_1.text.letter_space = 0;
    style0_label_1.text.line_space = 2;
    style0_label_1.text.opa = 255;

    label_1 = lv_label_create(parent, NULL);
    lv_label_set_text(label_1, "NEGATIVE PRESSURE");
    lv_label_set_align(label_1, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(label_1, LV_LABEL_LONG_BREAK);
    lv_obj_set_pos(label_1, 0, 4);
    lv_obj_set_size(label_1, 160, 19);
    lv_label_set_style(label_1, LV_LABEL_STYLE_MAIN, &style0_label_1);
#endif // LV_USE_LABEL

#ifdef LV_USE_CONT
    lv_style_copy(&style0_container_1, &lv_style_pretty);
    style0_container_1.body.main_color = lv_color_hex(0x0084ff);
    style0_container_1.body.grad_color = lv_color_hex(0x0084ff);
    style0_container_1.body.radius = 0;
    style0_container_1.body.border.width = 0;

    container_1 = lv_cont_create(parent, NULL);
    lv_obj_set_pos(container_1, 0, 0);
    lv_obj_set_size(container_1, 160, 26);
    lv_cont_set_style(container_1, LV_CONT_STYLE_MAIN, &style0_container_1);
#endif // LV_USE_CONT

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_2, &lv_style_transp);
    style0_label_2.text.color = lv_color_hex(0xffffff);
    style0_label_2.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_2.text.font = &lv_font_roboto_16;
    style0_label_2.text.letter_space = 0;
    style0_label_2.text.line_space = 2;
    style0_label_2.text.opa = 255;

    label_2 = lv_label_create(parent, NULL);
    lv_label_set_text(label_2, "NEGATIVE PRESSURE");
    lv_label_set_align(label_2, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(label_2, LV_LABEL_LONG_BREAK);
    lv_obj_set_pos(label_2, 0, 4);
    lv_obj_set_size(label_2, 160, 19);
    lv_label_set_style(label_2, LV_LABEL_STYLE_MAIN, &style0_label_2);
#endif // LV_USE_LABEL

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_3, &lv_style_transp);
    style0_label_3.text.color = lv_color_hex(0xffffff);
    style0_label_3.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_3.text.font = &lv_font_roboto_16;
    style0_label_3.text.letter_space = 0;
    style0_label_3.text.line_space = 2;
    style0_label_3.text.opa = 255;

    label_3 = lv_label_create(parent, NULL);
    lv_label_set_text(label_3, "PRESSURE OK");
    lv_label_set_align(label_3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(label_3, LV_LABEL_LONG_BREAK);
    lv_obj_set_pos(label_3, 0, 30);
    lv_obj_set_size(label_3, 160, 19);
    lv_label_set_style(label_3, LV_LABEL_STYLE_MAIN, &style0_label_3);
#endif // LV_USE_LABEL

#ifdef LV_USE_CONT
    lv_style_copy(&style0_container_3, &lv_style_pretty);
    style0_container_3.body.main_color = lv_color_hex(0x00cb00);
    style0_container_3.body.grad_color = lv_color_hex(0x00cb00);
    style0_container_3.body.radius = 0;
    style0_container_3.body.border.width = 0;

    container_3 = lv_cont_create(parent, NULL);
    lv_obj_set_pos(container_3, 0, 52);
    lv_obj_set_size(container_3, 160, 26);
    lv_cont_set_style(container_3, LV_CONT_STYLE_MAIN, &style0_container_3);
#endif // LV_USE_CONT

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_4, &lv_style_transp);
    style0_label_4.text.color = lv_color_hex(0xffffff);
    style0_label_4.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_4.text.font = &lv_font_roboto_16;
    style0_label_4.text.letter_space = 0;
    style0_label_4.text.line_space = 2;
    style0_label_4.text.opa = 255;

    label_4 = lv_label_create(parent, NULL);
    lv_label_set_text(label_4, "BATTERY IS GOOD");
    lv_label_set_align(label_4, LV_LABEL_ALIGN_CENTER);
    lv_label_set_long_mode(label_4, LV_LABEL_LONG_BREAK);
    lv_obj_set_pos(label_4, 0, 56);
    lv_obj_set_size(label_4, 160, 19);
    lv_label_set_style(label_4, LV_LABEL_STYLE_MAIN, &style0_label_4);
#endif // LV_USE_LABEL

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_5, &lv_style_transp);
    style0_label_5.text.color = lv_color_hex(0x303030);
    style0_label_5.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_5.text.font = &lv_font_roboto_16;
    style0_label_5.text.letter_space = 0;
    style0_label_5.text.line_space = 2;
    style0_label_5.text.opa = 255;

    label_5 = lv_label_create(parent, NULL);
    lv_label_set_text(label_5, "Pressure:");
    lv_obj_set_pos(label_5, 4, 83);
    lv_obj_set_size(label_5, 69, 19);
    lv_label_set_style(label_5, LV_LABEL_STYLE_MAIN, &style0_label_5);
#endif // LV_USE_LABEL

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_6, &lv_style_transp);
    style0_label_6.text.color = lv_color_hex(0x303030);
    style0_label_6.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_6.text.font = &lv_font_roboto_16;
    style0_label_6.text.letter_space = 0;
    style0_label_6.text.line_space = 2;
    style0_label_6.text.opa = 255;

    label_6 = lv_label_create(parent, NULL);
    lv_label_set_text(label_6, "Battery:");
    lv_obj_set_pos(label_6, 4, 103);
    lv_obj_set_size(label_6, 55, 19);
    lv_label_set_style(label_6, LV_LABEL_STYLE_MAIN, &style0_label_6);
#endif // LV_USE_LABEL

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_pressure, &lv_style_transp);
    style0_label_pressure.text.color = lv_color_hex(0x303030);
    style0_label_pressure.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_pressure.text.font = &lv_font_roboto_16;
    style0_label_pressure.text.letter_space = 0;
    style0_label_pressure.text.line_space = 2;
    style0_label_pressure.text.opa = 255;

    label_pressure = lv_label_create(parent, NULL);
    lv_label_set_text(label_pressure, "0 Pascal");
    lv_obj_set_pos(label_pressure, 76, 83);
    lv_obj_set_size(label_pressure, 61, 19);
    lv_label_set_style(label_pressure, LV_LABEL_STYLE_MAIN, &style0_label_pressure);
#endif // LV_USE_LABEL

#ifdef LV_USE_LABEL
    lv_style_copy(&style0_label_battery, &lv_style_transp);
    style0_label_battery.text.color = lv_color_hex(0x303030);
    style0_label_battery.text.sel_color = lv_color_hex(0x5596d8);
    style0_label_battery.text.font = &lv_font_roboto_16;
    style0_label_battery.text.letter_space = 0;
    style0_label_battery.text.line_space = 2;
    style0_label_battery.text.opa = 255;

    label_battery = lv_label_create(parent, NULL);
    lv_label_set_text(label_battery, "0 %");
    lv_obj_set_pos(label_battery, 64, 103);
    lv_obj_set_size(label_battery, 25, 19);
    lv_label_set_style(label_battery, LV_LABEL_STYLE_MAIN, &style0_label_battery);
#endif // LV_USE_LABEL

#ifdef LV_USE_IMG
    lv_style_copy(&style0_image_1, &lv_style_plain);

    image_1 = lv_img_create(parent, NULL);
    lv_obj_set_pos(image_1, 134, 102);
    lv_obj_set_size(image_1, 25, 25);
    lv_img_set_src(image_1, &alarm_on);
    lv_img_set_style(image_1, LV_IMG_STYLE_MAIN, &style0_image_1);
#endif // LV_USE_IMG
}
