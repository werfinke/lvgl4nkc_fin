#include <stdio.h>
#include <string.h>
#include "../../nkc_common/nkc/nkc.h"	
#include "../../m68k-elf/include/sys/m68k.h"

#include "./lvgl/lvgl.h"

//#include "C:/src/NKC-SMEDLIBS-master/NKC_UTIL.h"
//#include "C:/src/NKC-SMEDLIBS-master/NKC_GDP64.h"

#define GDP_MEM_PAGE0 0x800000
#define GDP_MEM_PAGE1 0x840000
#define GDP_MEM_PAGE2 0x880000
#define GDP_MEM_PAGE3 0x8C0000

#define LV_HOR_RES_MAX   512
#define LV_VER_RES_MAX   512

int tick = 0;

void doTick() {   
    tick++;           //for debug
    lv_tick_inc(5);   //Call this every 5 ms from a timer interrupt
}

void hardware_init() {
    gp_clearscreen();  //set video memory to black for all 4 pages
    _clock(&doTick);
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    //printf("%s\n", "disp_flush"); //for debug

    /* Copy px_map to your framebuffer here */
    for (int y=area->y1; y<=area->y2; y++) {
         memcpy((void*)GDP_MEM_PAGE0 + y*LV_HOR_RES_MAX + area->x1, px_map, (area->x2-area->x1+1));
    }

    lv_display_flush_ready(disp);
}


//static void lv_log_printf_cb(const char * buf)
//{
//    /* LVGL already formats the message */
//    printf("%s", buf);
//}

int main(int argc, char* argv[]) {

    DISABLE_CPU_INTERRUPTS;
    hardware_init();

    /* --------- Display buffer --------- */
    static lv_color_t buf1[LV_HOR_RES_MAX * 128];
    static lv_display_t *disp;

    printf("%s\n", "Hello from LVGL...");
    printf("%s %p\n", "GDP video memory at : ", (void*) GDP_MEM_PAGE0);
    printf("%s %p\n", "LVGL frame buffer at: ", (void*) &buf1);
    
    //memset(buf1, 0x00, sizeof(buf1)); //for debug
    
    //memset((void*)GDP_MEM_PAGE0, 0xFF, 512*256);
    //memset((void*)buf1, 0xFF, sizeof(buf1));

    printf("%s\n", "Logging is enabled");

    printf("%s\n", "do lv_init()...");
    lv_init();

   
  

    printf("%s\n", "do lv_display_create()...");
    disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    printf("%s %p\n", "display created: ",       (void*) disp);

    lv_display_set_flush_cb(disp, my_disp_flush);

    lv_display_set_buffers(
            disp,
            buf1,
            NULL,
            sizeof(buf1),
            LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    /*Change the active screen's background color*/
    //lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    //lv_obj_t * label = lv_label_create(lv_screen_active());
    //lv_label_set_text(label, "Hello world");
    //lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    //lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /* Create UI */
    printf("%s\n", "Create UI ...");
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);


    printf("%s\n", "do lvgl event loop...");
    
    ENABLE_CPU_INTERRUPTS; 
    while (tick < 1000) {

        if ((tick % 100) == 0) {
            printf("%s %i\n", "ticks:", tick);
        }
        
        if ((tick % 500) == 0) {
            memset(buf1, 0x55, sizeof(buf1));
            lv_refr_now(disp);
        }

        lv_timer_handler();   /* must be called periodically */
    }

    lv_refr_now(disp);


    printf("%s\n", "...bye !");

	return 0;
}