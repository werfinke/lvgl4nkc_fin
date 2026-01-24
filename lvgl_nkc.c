#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../nkc_common/nkc/nkc.h"	
#include "../../m68k-elf/include/sys/m68k.h"

#include "./lvgl/lvgl.h"

//#include "C:/src/NKC-SMEDLIBS-master/NKC_UTIL.h"
//#include "C:/src/NKC-SMEDLIBS-master/NKC_GDP64.h"

#define GDP_MEM_PAGE0 ((void *)0x800000u)
#define GDP_MEM_PAGE1 ((void *)0x840000u)
#define GDP_MEM_PAGE2 ((void *)0x880000u)
#define GDP_MEM_PAGE3 ((void *)0x8C0000u)

#define LV_HOR_RES_MAX   512
#define LV_VER_RES_MAX   256  //change in GP: T=IO setzen, Adr: $7f  Data:1

#define FB_STRIDE_BYTES 512

static lv_indev_t * indev_mouse;
volatile int16_t mouse_x = 0;
volatile int16_t mouse_y = 0;
volatile uint8_t old_mouse_keys;

#define CURSOR_FG_COL (uint8_t)32u
#define CURSOR_BG_COL (uint8_t)128u

const lv_image_dsc_t img_cursor;  /* your cursor image */

//muss woanders hin
#define BYTE_AT(adr) (*(unsigned char  volatile *) adr)

#define FPGA_GDP_FCOL_REG BYTE_AT(0xFFFFFF40)
#define FPGA_GDP_BCOL_REG BYTE_AT(0xFFFFFF42)

#define FPGA_GDP_CULT_REG     BYTE_AT(0xFFFFFF48)  // active color entry
#define FPGA_GDP_CULT_MSB_REG BYTE_AT(0xFFFFFF4A)  // 1bit
#define FPGA_GDP_CULT_LSB_REG BYTE_AT(0xFFFFFF4C)  // 8bit   */

#define FPGA_GDP_MODE BYTE_AT(0xFFFFFFEE)  //FPGA-GDP Graphics mode

static inline __attribute__((always_inline)) void GDP_WAIT(void) {
   while(!(GDP.cmd & 0x04u)) {};
}

void gp_gmode() {

    GDP_WAIT();
    FPGA_GDP_MODE = 0b00000001;

}

static void initCULT_local() {

  int MSB_col = 0;
  int LSB_col = 0;

  printf("%s\n", "CULT");
  for (int i=0; i < 16; i++) {
    GDP_WAIT(); FPGA_GDP_CULT_REG = i; 
    GDP_WAIT(); LSB_col = FPGA_GDP_CULT_LSB_REG;
    GDP_WAIT(); MSB_col = FPGA_GDP_CULT_MSB_REG;
    
    printf("%i:%u ", i, MSB_col + LSB_col);

  }
  printf("%s\n", "");
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x00; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00000000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x00 BLACK
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x01; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11111111; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;   //0x01 WHITE
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x02; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11111000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;   //0x02 YELLOW
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x03; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00111000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x03 LIME
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x04; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11000000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;   //0x04 RED
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x05; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00000111; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x05 BLUE
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x06; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11000111; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;  //0x06 FUCHSIA
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x07; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00111111; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;  //0x07 AQUA
  
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x08; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11000000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;   //0x04 RED

  //GDP_WAIT(); FPGA_GDP_CULT_REG = 0x08; 
  //GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b10010010; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;  //0x08 GRAY
  
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x09; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00100100; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x01;   //0x09 SILVER
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0a; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11011000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;  //0x0A OLIVE
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0b; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00011000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x0B GREEN
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0c; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11000000; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x0C MAROON
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0d; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00000011; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x0D NAVY
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0e; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b11000011; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0s0E PURPLE
  GDP_WAIT(); FPGA_GDP_CULT_REG = 0x0f; 
  GDP_WAIT(); FPGA_GDP_CULT_LSB_REG = 0b00011011; GDP_WAIT(); FPGA_GDP_CULT_MSB_REG = 0x00;   //0x0F TEAL

  printf("%s\n", "CULT");
  for (int i=0; i < 16; i++) {
    GDP_WAIT(); FPGA_GDP_CULT_REG = i; 
    GDP_WAIT(); LSB_col = FPGA_GDP_CULT_LSB_REG;
    GDP_WAIT(); MSB_col = FPGA_GDP_CULT_MSB_REG;
    
    printf("%i:%u ", i, MSB_col + LSB_col);

  }
  printf("%s\n", "");

  FPGA_GDP_BCOL_REG = WHITE;
  FPGA_GDP_FCOL_REG = YELLOW;
}

/* 8x8 arrow cursor, 1 byte per pixel (L8) */
const uint8_t cursor_8x8_map[] __attribute__((aligned(LV_DRAW_BUF_ALIGN))) = {
    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
    CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_FG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL, CURSOR_BG_COL,
};

const lv_image_dsc_t cursor_8x8 = {
    .header.cf = LV_COLOR_FORMAT_L8,
    .header.w = 8,
    .header.h = 8,
    .data_size = 64,
    .data = cursor_8x8_map,
};

void mouse_attach_cursor(void)
{
    lv_obj_t * cursor = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor, &cursor_8x8);

    /* Always on top */
    lv_obj_add_flag(cursor, LV_OBJ_FLAG_FLOATING);

    lv_indev_set_cursor(indev_mouse, cursor);
}

static lv_obj_t * label;

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

static uint8_t l8_to_332_lut[256];

int tick = 0;

void doTick() {   
    tick++;           //for debug
    lv_tick_inc(5);   //Call this every 5 ms from a timer interrupt
}

void hardware_init() {
    if (LV_VER_RES_MAX == 512) {GDP_WAIT(); FPGA_GDP_MODE = 0b00000001;}  //512 x 512 mode, obacht: colors change !
    gp_clearscreen();  
    initCULT_local();  //not working
    _clock(&doTick); 
}

static inline uint8_t l8_to_rgb332(uint8_t v)
{
    /* Generated by ChatGPT */
    /* L8 grayscale → RGB332 */
    uint8_t r = v & 0xE0;
    uint8_t g = (v & 0xE0) >> 3;
    uint8_t b = (v & 0xC0) >> 6;
    return r | g | b; 
}

void lut_init(void)
{
    /* Generated by ChatGPT */
    for(int i = 0; i < 256; i++) {
        uint8_t v = (uint8_t)i;
        l8_to_332_lut[i] = (uint8_t)((v & 0xE0) | ((v & 0xE0) >> 3) | (v >> 6));
        //l8_to_332_lut[i] = (uint8_t)( (v * 0x100 * 0x100) + (v * 0x100) + v);
    }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /* Copy px_map to your framebuffer here */
    //for (int y=area->y1; y<=area->y2; y++) {
    //     memcpy((void*)GDP_MEM_PAGE0 + y*LV_HOR_RES_MAX + area->x1, px_map, (area->x2-area->x1+1));
    //}

    /* Generated by ChatGPT */
    // px_map points to the rendered rectangle in row-major order
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    for(int32_t y = 0; y < h; y++) {
        
        uint8_t * src = px_map + y * w;

        int32_t fb_y = (LV_VER_RES_MAX - 1) - (area->y1 + y);
        uint8_t *dst = (uint8_t*)GDP_MEM_PAGE0
                     + fb_y * FB_STRIDE_BYTES
                     + area->x1;

        memcpy(dst, src, w);
    }
    
    /* all colors on screen */
    /*for (int i=0; i < 256; i++) {
        memset((void*)GDP_MEM_PAGE0+(512*  i), i, 512);
    }*/

    /*debug - to see disp_flush runs, as per defined in BUF_LINES
    gp_moveto((int)area->x1, 256-(int)area->y1);
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
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * slider = lv_event_get_target_obj(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(label, "%" LV_PRId32, (int)lv_slider_get_value(slider));
        lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
    }
}


int main(int argc, char* argv[]) {

    DISABLE_CPU_INTERRUPTS;
    hardware_init();

    #define BUF_LINES 32
    static uint8_t buf1[LV_HOR_RES_MAX * BUF_LINES] __attribute__((aligned(LV_DRAW_BUF_ALIGN)));
    static lv_display_t *disp;

    printf("%s %p\n", "GDP video memory at : ", (void*) GDP_MEM_PAGE0);
    printf("%s %p\n", "LVGL frame buffer at: ", (void*) &buf1);
    lv_init();

    disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_L8);
    //lut_init();

    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x808080), LV_PART_MAIN);
    lv_obj_set_style_text_font(lv_screen_active(), &lv_font_montserrat_10, LV_PART_MAIN);

    //define mouse, display needs to be created first
    indev_mouse = lv_indev_create();
    lv_indev_set_type(indev_mouse, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_mouse, mouse_read_cb);

    lv_obj_t * cursor = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor, &img_cursor);
    lv_obj_add_flag(cursor, LV_OBJ_FLAG_FLOATING);
    lv_indev_set_cursor(indev_mouse, cursor);

    mouse_attach_cursor();

    /*Create UI */
    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(label1, "TOP_LEFT");
    lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_text(label2, "TOP_MID");
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * label3 = lv_label_create(lv_screen_active());
    lv_label_set_text(label3, "TOP_RIGHT");
    lv_obj_align(label3, LV_ALIGN_TOP_RIGHT, 0, 0);


    lv_obj_t * label4 = lv_label_create(lv_screen_active());
    lv_label_set_text(label4, "BOTTOM_LEFT");
    lv_obj_align(label4, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t * label5 = lv_label_create(lv_screen_active());
    lv_label_set_text(label5, "BOTTOM_MID");
    lv_obj_align(label5, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t * label6 = lv_label_create(lv_screen_active());
    lv_label_set_text(label6, "BOTTOM_RIGHT");
    lv_obj_align(label6, LV_ALIGN_BOTTOM_RIGHT, 0, 0);


    lv_obj_t * label7 = lv_label_create(lv_screen_active());
    lv_label_set_text(label7, "LEFT_MID");
    lv_obj_align(label7, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * label9 = lv_label_create(lv_screen_active());
    lv_label_set_text(label9, "RIGHT_MID");
    lv_obj_align(label9, LV_ALIGN_RIGHT_MID, 0, 0);


    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "NKC");                     /*Set the labels text*/
    lv_obj_center(label);


    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider, 200);                          /*Set the width*/
    lv_obj_center(slider);                                  /*Align to the center of the parent (screen)*/
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /*Assign an event function*/
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x010101), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xaaaaaa), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x555555), LV_PART_KNOB);





    /*Create a label above the slider*/
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "0");
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/


    printf("%s\n", "do lvgl event loop...");
    
    tick = 0;
    ENABLE_CPU_INTERRUPTS; 

    while (tick < 10000) {

        //do someting

        lv_timer_handler();   /* must be called periodically */
    }

    GDP_WAIT(); FPGA_GDP_MODE = 0b00000000;  //switch back to 512*256
    gp_clearscreen();
	return 0;
}