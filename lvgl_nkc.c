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
#define LV_VER_RES_MAX   256

#define FB_STRIDE_BYTES 512

static uint8_t l8_to_332_lut[256];

int tick = 0;

void doTick() {   
    tick++;           //for debug
    lv_tick_inc(5);   //Call this every 5 ms from a timer interrupt
}

void hardware_init() {
    gp_clearscreen();  //set video memory to black for all 4 pages
    _clock(&doTick);
}

static inline uint8_t l8_to_rgb332(uint8_t v)
{
    /* grayscale → RGB332 */
    uint8_t r = v & 0xE0;
    uint8_t g = (v & 0xE0) >> 3;
    uint8_t b = (v & 0xC0) >> 6;
    return r | g | b;
}

void lut_init(void)
{
    for(int i = 0; i < 256; i++) {
        uint8_t v = (uint8_t)i;
        l8_to_332_lut[i] = (uint8_t)((v & 0xE0) | ((v & 0xE0) >> 3) | (v >> 6));
    }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    //printf("%s\n", "disp_flush"); //for debug

    /* Copy px_map to your framebuffer here */
    //for (int y=area->y1; y<=area->y2; y++) {
    //     memcpy((void*)GDP_MEM_PAGE0 + y*LV_HOR_RES_MAX + area->x1, px_map, (area->x2-area->x1+1));
    //}

    //printf("flush: x=%d..%d y=%d..%d\n", (int)area->x1, (int)area->x2, (int)area->y1, (int)area->y2);
    //printf("%s %p\n", "LVGL FB at: ", (void*) px_map);

    // px_map points to the rendered rectangle in row-major order
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    #define FB_BASE_ADDR ((uintptr_t)GDP_MEM_PAGE0)
    //static uint8_t * const fb = (uint8_t *)FB_BASE_ADDR;

    for(int32_t y = 0; y < h; y++) {
        //uint8_t * dst = fb + (area->y1 + y) * FB_STRIDE_BYTES + area->x1;
        uint8_t * src = px_map + y * w;

        int32_t fb_y = (LV_VER_RES_MAX - 1) - (area->y1 + y);
        uint8_t *dst = (uint8_t*)GDP_MEM_PAGE0
                     + fb_y * FB_STRIDE_BYTES
                     + area->x1;

        for(int32_t x = 0; x < w; x++) {
            dst[x] = l8_to_rgb332(src[x]);
        }
    }
    
    //debug
    //gp_moveto((int)area->x1, (int)area->y1);
    //gp_drawto((int)area->x1, (int)area->y2);
    //gp_drawto((int)area->x2, (int)area->y2);
    //gp_drawto((int)area->x2, (int)area->y1);
    //gp_drawto((int)area->x1, (int)area->y1);
    //DebugPrintPos(((area->x1 + area->x2)/2)-21, ((area->y1 + area->y2)/2)-4, "", tick, 0);

    lv_display_flush_ready(disp);
}

int main(int argc, char* argv[]) {

    DISABLE_CPU_INTERRUPTS;
    hardware_init();

    #define BUF_LINES 16
    static uint8_t buf1[LV_HOR_RES_MAX * BUF_LINES];
    static lv_display_t *disp;

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
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_L8);
    lut_init();

    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);

    /*Change the active screen's background color*/
    //lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x80), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/

    printf("%s\n", "Create UI ...");

    //lv_obj_t * label1 = lv_label_create(lv_screen_active());
    //lv_label_set_text(label1, "Hello world");
    //lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    //lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

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

    lv_obj_t * label8 = lv_label_create(lv_screen_active());
    lv_label_set_text(label8, "CENTER");
    lv_obj_align(label8, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label9 = lv_label_create(lv_screen_active());
    lv_label_set_text(label9, "RIGHT_MID");
    lv_obj_align(label9, LV_ALIGN_RIGHT_MID, 0, 0);

    printf("%s\n", "do lvgl event loop...");
    
    tick = 0;
    ENABLE_CPU_INTERRUPTS; 

    while (tick < 5000) {

        //do someting

        lv_timer_handler();   /* must be called periodically */
    }

	return 0;
}