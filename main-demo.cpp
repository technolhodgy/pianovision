#include "pico/stdlib.h"
#include <cstdio>
#include <iterator>
#include <sstream>
#include "hardware/gpio.h"

#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"


#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 1080

using namespace pimoroni;

DVDisplay display;
PicoGraphics_PenDV_RGB555 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

Pen BLACK = graphics.create_pen(0, 0, 0);
Pen WHITE = graphics.create_pen(255, 255, 255);
int f,x,y;

int height = 30;

uint colours[16][3] = {
   {0,0,0},{0,0,192},{192,0,0},{192,0,192},{0,192,0},{0,192,192},{192,192,0},{192,192,192},
   {64,64,64},{0,0,255},{255,0,0},{255,0,255},{0,255,0},{0,255,255},{255,255,0},{255,255,255}
   
};

int main() {
   stdio_init_all();
   printf("Hello World\n");

   
   printf("Init Video...\n");
   display.preinit();
   display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, DVDisplay::MODE_RGB555, FRAME_WIDTH, FRAME_HEIGHT);
   printf("Done!\n");
   
   constexpr uint BUTTON_Y = 9;
   gpio_init(BUTTON_Y);
   gpio_set_dir(BUTTON_Y, GPIO_IN);
   gpio_pull_up(BUTTON_Y);
   
   for (f = 0; f<2;f++)
   {
      display.set_scroll_idx_for_lines(1,   0, 160);
      display.set_scroll_idx_for_lines(2, 160, 320);
      display.set_scroll_idx_for_lines(3, 320, 480);
      //display.set_scroll_idx_for_lines(4, 440, 480);
      display.setup_scroll_group(Point(0,0),1,640,160,0,0);
      display.setup_scroll_group(Point(0,0),2,640,320,0,160);
      display.setup_scroll_group(Point(0,0),3,640,480,0,320);
      //display.setup_scroll_group(Point(0,320),4,640,500,0,440);
      graphics.set_pen(BLACK);
      graphics.clear();
      graphics.set_pen(WHITE);
      graphics.text("Hello PicoVision Boilerplate!", Point(0, 0), FRAME_WIDTH);
      display.flip();
   }
   while ( gpio_get(BUTTON_Y ) )
   {}

   for (f = 0; f<2;f++)
   {
         for (int x =0 ; x <16 ; x ++)
         {
            for (int y =0 ; y <16 ; y ++)
            {
               graphics.set_pen(graphics.create_pen(colours[x][0],colours[x][1],colours[x][2]));
               graphics.rectangle({x*40,y*height,40,height});
               graphics.set_pen(graphics.create_pen(colours[y][0],colours[y][1],colours[y][2]));
               graphics.text("Ahoy", Point(x*40, y*height+8), FRAME_WIDTH);
            }
         }
      display.flip();
   }
      while(true) {
      for (x=0; x <160;x++)
      {
         //for (f = 0; f<2;f++)
         //{
            display.setup_scroll_group(Point(0,    x),1,0,160,640,0);
            display.setup_scroll_group(Point(0,320-x),2,0,320,640,160);
            display.setup_scroll_group(Point(0,    x),3,0,480,640,320);
            //display.setup_scroll_group(Point(0,    0),4,0,500,640,440);
            //display.flip();
            sleep_ms(15);
         //}
      }
   }
}
