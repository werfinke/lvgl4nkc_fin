#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
 *  BASIC SETTINGS
 *====================*/

/* No OS */
#define LV_USE_OS           LV_OS_NONE

/*====================
 *  COLOR SETTINGS
 *====================*/

/* 8-bit indexed / RGB332 */
#define LV_COLOR_DEPTH      8
#define LV_COLOR_16_SWAP    0
#define LV_COLOR_SCREEN_TRANSP 0

/*====================
 *  MEMORY SETTINGS
 *====================*/

/* Use LVGL internal heap */
#define LV_MEM_CUSTOM       0

/* Minimal heap – increase if needed */
#define LV_MEM_SIZE         (96U * 1024U)

#define LV_MEM_ATTR
#define LV_MEM_ADR	0x00c0000

/*====================
 *  HAL SETTINGS
 *====================*/

#define LV_DISP_DEF_REFR_PERIOD   30
#define LV_INDEV_DEF_READ_PERIOD 30

/*====================
 *  DRAWING SETTINGS
 *====================*/

#define LV_DRAW_COMPLEX     0   /* No shadows, gradients, transforms */
#define LV_USE_BLEND_MODES  0
#define LV_USE_OPA          0

/*====================
 *  FEATURE SETTINGS
 *====================*/

#define LV_USE_LOG          0
#define LV_USE_ASSERT_NULL  0
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0

/* No animations */
#define LV_USE_ANIMATION    0

/* No themes */
#define LV_USE_THEME_DEFAULT 0

/*====================
 *  WIDGETS (minimal)
 *====================*/

#define LV_USE_OBJ          1
#define LV_USE_LABEL        1
#define LV_USE_BTN          1

/* Disable everything else */
#define LV_USE_ARC          1
#define LV_USE_BAR          0
#define LV_USE_CANVAS       0
#define LV_USE_CHECKBOX     0
#define LV_USE_DROPDOWN     1
#define LV_USE_IMG          0
#define LV_USE_LINE         0
#define LV_USE_ROLLER       0
#define LV_USE_SLIDER       0
#define LV_USE_SWITCH       0
#define LV_USE_TABLE        0
#define LV_USE_TEXTAREA    1

/*====================
 *  FONTS
 *====================*/

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_DEFAULT       &lv_font_montserrat_12

/* Disable all other fonts */
#define LV_FONT_MONTSERRAT_14 0
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0

/*====================
 *  TEXT SETTINGS
 *====================*/

#define LV_TXT_ENC          LV_TXT_ENC_ASCII
#define LV_TXT_BREAK_CHARS  " ,.;:-_"

/*====================
 *  DEBUG
 *====================*/

#define LV_USE_DEBUG        0

#endif /* LV_CONF_H */
