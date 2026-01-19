LVGL Grafikbibliothek adaptiert für den 68k NKC auf FPGA-GDP Basis ("TangNano NKC", "SOC NKC") 

How to use:
1) im Verzeichnis ../lvgl4nkc/lvgl
   
   make            
   make install

3) im Verzeichnis ../lvgl4nkc
   
   make

5) .m68 executable ist in .../lvgl4nkc/_out

Infos
   - lv_conf.h line 443: #define LV_USE_LOG 1  //Logging ist eingeschaltet und wird ueber printf ausgegeben
   - lvgl_nkc.c line 94: nuetzlich um aktuellen disp_flush() Bereich zu sehen
   - FPGA-GDP video memory hat 8 bit color (look-up von 9 bit), daher ist LVGL als L8 color format konfiguriert
   - Calendar widget wird nicht korrekt plaziert, y pos falsch
   - rudimentärer NKC Maus support implementiert, noch ohne click
     
Mehr Infos zu LVGL gibt's hier: https://lvgl.io/

