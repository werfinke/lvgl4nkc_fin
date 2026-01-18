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
   - lvgl_nkc.c line 94: nuetzlich to see multiple disp_flush runs, as per defined in BUF_LINES
   - FPGA-GDP video memory hat color format RGB332, LVGL 9.5 ueterstuetzt dies nicht mehr direkt, Situation ist nicht ganz klar daher ist LVGL bis auf Weiteres als L8 color format (greyscale) konfiguriert, zZT sind die Farben nicht korrekt, und disp_flush ist lahm wg Konvertierung jedes Pixels von L8 in RGB332. eventuell ost es notwendig auf LVGL 8.3.11 umzusteigen, oder das Format de GDP-FPGA auf ein LVGL unterstütztes umzustellen
   - Calendar widget wird nicht korrekt plaziert, y pos falsch
   - rudimentärer NKC Maus support implementiert
     
Mehr Infos zu LVGL gibt's hier: https://lvgl.io/

*
