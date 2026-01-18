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
   - FPGA-GDP framebuffer hat color format RGB332, LVGL ueterstuetzt dies scheinbar nicht direkt (sagt ChatGPT), Situation ist nicht ganz klar daher ist LVGL bis auf Weiteres als L8 color format konfiguriert, zZT sind die Farben nicht korrekt, und disp_flush ist lahm wg Konvertierung jedes Pixels
   - Calendar widget wird nicht korrekt plaziert, y pos falsch
   - NKC Maus noch nicht in LVGL eventloop eingehaengt, ebenso noch kein Mauszeiger
     
Mehr Infos zu LVGL gibt's hier: https://lvgl.io/
