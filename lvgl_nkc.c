
#include <stdio.h>
#include "../../nkc_common/nkc/nkc.h"	
#include "../../m68k-elf/include/sys/m68k.h"
   
#include "./lvgl/lvgl.h"

//#include "./NKC_FPGA_TIMER.h"

extern int _int_handler_sta;  //address of interrupt handler

//#define FB_ADDR   ((uint8_t *)0x800000)
//#define HOR_RES   512
//#define VER_RES   256

void hardware_init() {

    //Timer and Interrupt
    DISABLE_CPU_INTERRUPTS; //defined in ..m68k-elf/m68k-elf/include/sys/m68k.h

    //Timer_stop();
    //Timer_load((word)1000);  //Time runs at 1Mhz, 1ms = 1/1e3 Mhz
    //Timer_enable_interrupt(TRUE);

    ENABLE_CPU_INTERRUPTS;

    //Timer_run();

    //GDP video  
    gp_clearscreen();  //set video memory to black for all 4 pages
}

int ticks = 0;

void  __attribute__((interrupt)) __attribute__((section(".interrupt"))) int_handler_func() {
    
    ticks++; //for debug
    lv_tick_inc(1);   //Call this every 1 ms from a timer interrupt
}


int main(int argc, char* argv[]) {

    hardware_init();

    printf("%s\n", "Hello from LVGL...");
    printf("%s %p\n", "Interrupt handler at: ", (void*)_int_handler_sta);
    
    //to do
    printf("%s %p\n", "GDP video memory at : ", (void*) 0);
    printf("%s %p\n", "LVGL frame buffer at: ", (void*) 0);
    printf("%s %p\n", "LVGL heap at: ",         (void*) 0);
    printf("%s %p\n", "LVGL heap size: ",       (void*) 0);

    //lv_port_disp_init();

    /* Create UI */
    //lv_obj_t *label = lv_label_create(lv_scr_act());
    //lv_label_set_text(label, "Hello LVGL");
    //lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    //while (1) {

    //erst mal 100x tick und schaun was passiert...
    for (int i=0; i < 100; i++) {
        //lv_tick_inc(1);
        //lv_timer_handler();
        //delay_ms(5);
    }

    //}

    printf("%s\n", "...bye !");

	return 0;

}