#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 *      INCLUDES
 **********************/

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif
/**********************
 *       WIDGETS
 **********************/
lv_style_t style0_container_1;
lv_style_t style0_label_1;
lv_style_t style0_container_2;
lv_style_t style0_label_2;
lv_style_t style0_label_3;
lv_style_t style0_container_3;
lv_style_t style0_label_4;
lv_style_t style0_label_5;
lv_style_t style0_label_6;
lv_style_t style0_label_pressure;
lv_style_t style0_label_battery;
lv_style_t style0_image_1;

lv_obj_t *label_1;
lv_obj_t *label_2;
lv_obj_t *label_3;
lv_obj_t *label_4;
lv_obj_t *label_5;
lv_obj_t *label_6;
lv_obj_t *label_pressure;
lv_obj_t *label_battery;
lv_obj_t * container_1;
lv_obj_t * container_2;
lv_obj_t * container_3;
lv_obj_t *image_1;

LV_IMG_DECLARE(alarm_off);
LV_IMG_DECLARE(alarm_on);
/**********************
 * GLOBAL PROTOTYPES
 **********************/

void app_create(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*APP_H*/
