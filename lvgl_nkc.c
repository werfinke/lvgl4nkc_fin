
#include <stdio.h>
#include "../../nkc_common/nkc/nkc.h"	
   

#include "./lvgl/lvgl.h"

//#define FB_ADDR   ((uint8_t *)0x800000)
//#define HOR_RES   512
//#define VER_RES   256

//Call this every 1 ms from a timer interrupt:
/*
void SysTick_Handler(void)
{
    lv_tick_inc(1);
}*/


int main(int argc, char* argv[]) {

	gp_clearscreen();
    printf("%s\n", "Hello from LVGL...");

 	//hardware_init();
    //lv_port_disp_init();

    /* Create UI */
    //lv_obj_t *label = lv_label_create(lv_scr_act());
    //lv_label_set_text(label, "Hello LVGL");
    //lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //while (1) {

    //erst mal 100x tick und schaun was passiert...
    for (int i=0; i < 100; i++) {
        lv_tick_inc(1);
        //lv_timer_handler();
        //delay_ms(5);
    }

    //}

    printf("%s\n", "...bye !");

	return 0;

}