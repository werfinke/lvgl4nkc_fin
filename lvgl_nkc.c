#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../nkc_common/nkc/nkc.h"   
#include "../../m68k-elf/include/sys/m68k.h"
#include "lv_color_565_to_gdp.h"

#include "./lvgl/lvgl.h"

//#include "C:/src/NKC-SMEDLIBS-master/NKC_UTIL.h"
//#include "C:/src/NKC-SMEDLIBS-master/NKC_GDP64.h"

#define GDP_MEM_PAGE0 ((void *)0x800000u)
#define GDP_MEM_PAGE1 ((void *)0x840000u)
#define GDP_MEM_PAGE2 ((void *)0x880000u)
#define GDP_MEM_PAGE3 ((void *)0x8C0000u)

#define LV_HOR_RES_MAX   512
#define LV_VER_RES_MAX   512

#define FB_STRIDE_BYTES 512

static lv_indev_t * indev_mouse;
volatile int16_t mouse_x = 0;
volatile int16_t mouse_y = 0;
volatile uint8_t old_mouse_keys;

#define BYTE_AT(adr) (*(unsigned char  volatile *) adr)

static lv_indev_t * indev_mouse;
static lv_indev_t * indev_keyboard;

#define CURSOR_FG_COL (uint16_t)0xFFFFu
#define CURSOR_BG_COL (uint16_t)0x8410u

#define FPGA_GDP_MODE   BYTE_AT(0xFFFFFEFE)  //FPGA-GDP Graphics mode
#define GDP_HSCROLL_REG BYTE_AT(0xFFFFFEC2)  //Hardware scroll
#define GDP_CTRL1_REG   BYTE_AT(0xFFFFFEE2)   

const lv_image_dsc_t img_cursor_16;  /* your cursor image */

/* 16x16 mouse cursor, L8 */
static const uint16_t cursor_map_l8[16 * 16] __attribute__((aligned(LV_DRAW_BUF_ALIGN))) = {

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,

};

const lv_image_dsc_t img_mouse_cursor = {
    .header = {
        .cf = LV_COLOR_FORMAT_RGB565,
        .w  = 16,
        .h  = 16,
        .stride = 16,
        .magic = LV_IMAGE_HEADER_MAGIC
    },
    .data_size = sizeof(cursor_map_l8),
    .data = cursor_map_l8,
};

void mouse_attach_cursor(void)
{
    lv_obj_t * cursor = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor, &img_mouse_cursor);

    /* Always on top */
    lv_obj_add_flag(cursor, LV_OBJ_FLAG_FLOATING);

    lv_indev_set_cursor(indev_mouse, cursor);
}

static lv_obj_t * label;


static void keyboard_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    (void) indev;
    char ch_key = 0x0;
    if(gp_csts() != 0x0) {
      data->state =  LV_INDEV_STATE_PRESSED;
      ch_key = gp_ci();
      data->key = ch_key;
      // printf("Piiep: %x\n", ch_key);
      ch_key = 0x0;
    } else {
      data->state =  LV_INDEV_STATE_RELEASED;
    }
}
static void mouse_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    //(void) indev;

    static int16_t dx = 0;
    static int16_t dy = 0;
    const uint8_t keys = gp_get_mouse(&dx, &dy);  //rel. change since last poll
   
    mouse_x += dx;
    mouse_y += -dy;

    if(mouse_x < 0) mouse_x = 0;
    if(mouse_y < 0) mouse_y = 0;
    if(mouse_x >= (int16_t)LV_HOR_RES_MAX) mouse_x = (int16_t)LV_HOR_RES_MAX - 1;
    if(mouse_y >= (int16_t)LV_VER_RES_MAX) mouse_y = (int16_t)LV_VER_RES_MAX - 1;

    data->point.x = mouse_x;  
    data->point.y = mouse_y;

    if (((keys & ~old_mouse_keys) & L_BUTTON) != 0u) {
        data->state = LV_INDEV_STATE_PRESSED;    
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->continue_reading = false;

    old_mouse_keys = keys;
}

int tick = 0;

void doTick() {   
    tick++;           //for debug
    lv_tick_inc(5);   //Call this every 5 ms from a timer interrupt
}

void hardware_init() {
    if (LV_VER_RES_MAX  == 512) FPGA_GDP_MODE = 0b00000001;
    gp_clearscreen();  //set video memory to black for all 4 pages
    _clock(&doTick);
}

static inline uint8_t rgb565_to_nkc(uint16_t v)
{
    /* Generated by ChatGPT */
    uint16_t r = (v & 0xE000) >> 6;
    uint16_t g = (v & 0x0780) >> 4;
    uint16_t b = (v & 0x001C) >> 2;
    return rgb565_to_nkc_256lut[(r|g|b)];
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint16_t *px_map)
{
    //printf("%s\n", "disp_flush"); //for debug

    /* Copy px_map to your framebuffer here */
    //for (int y=area->y1; y<=area->y2; y++) {
    //     memcpy((void*)GDP_MEM_PAGE0 + y*LV_HOR_RES_MAX + area->x1, px_map, (area->x2-area->x1+1));
    //}

    //printf("flush: x=%d..%d y=%d..%d\n", (int)area->x1, (int)area->x2, (int)area->y1, (int)area->y2);
    //printf("%s %p\n", "LVGL FB at: ", (void*) px_map);

    //GDP_CTRL1_REG |= 0b00000100;  //GDP Beam off

    /* Generated by ChatGPT */
    // px_map points to the rendered rectangle in row-major order
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    #define FB_BASE_ADDR ((uintptr_t)GDP_MEM_PAGE0)
    //static uint8_t * const fb = (uint8_t *)FB_BASE_ADDR;

    for(int32_t y = 0; y < h; y++) {
        //uint8_t * dst = fb + (area->y1 + y) * FB_STRIDE_BYTES + area->x1;
        uint16_t * src = px_map + y * w;

        int32_t fb_y = (LV_VER_RES_MAX - 1) - (area->y1 + y);
        uint8_t *dst = (uint8_t*)GDP_MEM_PAGE0
                     + fb_y * FB_STRIDE_BYTES
                     + area->x1;

        for(int32_t x = 0; x < w; x++) {
            dst[x] = rgb565_to_nkc(src[x]);
        }
     //   memcpy(dst,src,w);
    }
    
    //GDP_CTRL1_REG |= 0b11111011; //GDP Beam on
    GDP_HSCROLL_REG = 0; //


    //debug - to see multiple disp_flush runs, as per defined in BUF_LINES
    /*gp_moveto((int)area->x1, 256-(int)area->y1);
    gp_drawto((int)area->x1, 256-(int)area->y2);
    gp_drawto((int)area->x2, 256-(int)area->y2);
    gp_drawto((int)area->x2, 256-(int)area->y1);
    gp_drawto((int)area->x1, 256-(int)area->y1);*/
    
    lv_display_flush_ready(disp);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "NKC: %d", cnt);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);

    /*Refresh the text*/
    //lv_label_set_text_fmt(label, "%" LV_PRId32, lv_slider_get_value(slider));

    lv_obj_t * label  = lv_event_get_user_data(e);
    lv_label_set_text_fmt(label, "%" LV_PRId32, lv_slider_get_value(slider));
    
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}


int main(int argc, char* argv[]) {

    DISABLE_CPU_INTERRUPTS;
    hardware_init();

    #define BUF_LINES 32
    static uint16_t buf1[LV_HOR_RES_MAX * BUF_LINES] __attribute__((aligned(LV_DRAW_BUF_ALIGN)));
    static lv_display_t *disp;

    LV_FONT_DECLARE(my_open_sans_reg_20);
    LV_FONT_DECLARE(my_open_sans_reg_14);
    printf("%s %p\n", "GDP video memory at : ", (void*) GDP_MEM_PAGE0);
    printf("%s %p\n", "LVGL frame buffer at: ", (void*) &buf1);
        
    //testing, put someting in buffer
    //memset((void*)buf1, 0xFF, sizeof(buf1));
    
    //testing, put someting in GDPvideo ram
    //memset((void*)GDP_MEM_PAGE0+(512*  0), 0x55, 512);
    //memset((void*)GDP_MEM_PAGE0+(512*128), 0x55, 512);
    //memset((void*)GDP_MEM_PAGE0+(512*256), 0x55, 512);

    lv_init();

    disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);
    //lv_obj_set_style_text_font(lv_screen_active(), &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_font(lv_screen_active(), &my_open_sans_reg_14, 0);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    //define mouse, display needs to be created first
    indev_mouse = lv_indev_create();
    lv_indev_set_type(indev_mouse, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_mouse, mouse_read_cb);
    indev_keyboard = lv_indev_create();
    lv_indev_set_type(indev_keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_keyboard, keyboard_read_cb);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(indev_keyboard, g);

    lv_obj_t * cursor = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor, &img_cursor_16);
    lv_obj_add_flag(cursor, LV_OBJ_FLAG_FLOATING);
    lv_indev_set_cursor(indev_mouse, cursor);

    mouse_attach_cursor();






    /*Create UI */
    /*lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(label1, "TOP_LEFT");
    lv_obj_set_style_text_color(label1, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN); 
    lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_text(label2, "TOP_MID");
    lv_obj_set_style_text_color(label2, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN); 
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * label3 = lv_label_create(lv_screen_active());
    lv_label_set_text(label3, "TOP_RIGHT");
    lv_obj_set_style_text_color(label3, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN); 
    lv_obj_align(label3, LV_ALIGN_TOP_RIGHT, 0, 0);


    lv_obj_t * label4 = lv_label_create(lv_screen_active());
    lv_label_set_text(label4, "BOTTOM_LEFT");
    lv_obj_set_style_text_color(label4, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN); 
    lv_obj_align(label4, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t * label5 = lv_label_create(lv_screen_active());
    lv_label_set_text(label5, "BOTTOM_MID");
    lv_obj_set_style_text_color(label5, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN); 
    lv_obj_align(label5, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t * label6 = lv_label_create(lv_screen_active());
    lv_label_set_text(label6, "BOTTOM_RIGHT");
    lv_obj_set_style_text_color(label6, lv_palette_main(LV_PALETTE_CYAN), LV_PART_MAIN); 
    lv_obj_align(label6, LV_ALIGN_BOTTOM_RIGHT, 0, 0);


    lv_obj_t * label7 = lv_label_create(lv_screen_active());
    lv_label_set_text(label7, "LEFT_MID");
    lv_obj_set_style_text_color(label7, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN); 
    lv_obj_align(label7, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * label9 = lv_label_create(lv_screen_active());
    lv_label_set_text(label9, "RIGHT_MID");
    lv_obj_set_style_text_color(label9, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN); 
    lv_obj_align(label9, LV_ALIGN_RIGHT_MID, 0, 0);*/

    //NKC button
    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 20);                            /*Set its position*/
    lv_obj_set_size(btn, 60, 40);                          /*Set its size*/
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "NKC");                     /*Set the labels text*/
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN); 
    lv_obj_center(label);

    //button with gradient
    lv_obj_t * btn8 = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn8, 210, 80);                            /*Set its position*/
    lv_obj_set_size(btn8, 60, 40);                          /*Set its size*/
    lv_obj_set_style_bg_color(btn8, lv_palette_main(LV_PALETTE_CYAN), LV_PART_MAIN);

    //lv_obj_set_style_bg_color(btn8, lv_color_hex(0xFF0000), 0);       /* start */
    //lv_obj_set_style_bg_grad_color(btn8, lv_color_hex(0x0000FF), 0);  /* end */
    //lv_obj_set_style_bg_grad_dir(btn8, LV_GRAD_DIR_HOR, 0);           /* horizontal */


    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider, 200);                          /*Set the width*/
    lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN); 
    lv_obj_center(slider);                                  /*Align to the center of the parent (screen)*/
   
    /*Create a label above the slider*/
    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(label1, "0");
    lv_obj_set_style_text_color(label1, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN); 
    lv_obj_align_to(label1, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/

    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, label1);     /*Assign an event function*/

    //gp_clearscreen();  //set video memory to black for all 4 pages
    tick = 0;
    ENABLE_CPU_INTERRUPTS; 

    while (tick < 15000) {

        //do someting

        lv_timer_handler();   /* must be called periodically */
    }

    FPGA_GDP_MODE = 0b00000000;  //FPGA-GDP Graphics mode switch back to 512x256
	return 0;
}
