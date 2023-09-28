#include "pico/stdlib.h"
#include <cstdio>
#include <iterator>
#include <sstream>
#include <string>
#include <iostream>

#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics_dv.hpp"

#include "ff.h"



#include "hardware/dma.h"

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 1080



using namespace pimoroni;

FATFS fs;
FIL fil;
FIL audio_file;
FRESULT fr;


char midifiles[30][10];
char text[30];

#define NUM_BUFFERS 1
#define BUFFER_LEN 32768
#define BUFFER_BYTES (BUFFER_LEN*2)
unsigned char  buf[NUM_BUFFERS][BUFFER_LEN];
volatile uint write_buf;
volatile uint read_buf;
uint buf_idx;

int f,x,y,l;
int b =0;

DVDisplay display;
PicoGraphics_PenDV_P5 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

#include"setup.cpp"

//Pen BLACK = graphics.create_pen(0, 0, 0);
//Pen WHITE  = graphics.create_pen(255, 255, 255);

static void fill_file_buffer() {
    uint next_buf_idx = (write_buf + 1) & 0xF;
    if (next_buf_idx == read_buf) return;

    uint bytes_read;
    fr = f_read(&fil, buf[write_buf], BUFFER_BYTES, &bytes_read);
    if (fr != FR_OK) {
        printf("Failed to read data, error: %d\n", fr);
        return;
    }

    write_buf = next_buf_idx;
}

int main() 
{
   stdio_init_all();
     
   constexpr uint BUTTON_Y = 9;
   gpio_init(BUTTON_Y);
   gpio_set_dir(BUTTON_Y, GPIO_IN);
   gpio_pull_up(BUTTON_Y);
   printf("Hello World\n");

   printf("Init Video...\n");
   display.preinit();
   //display.init(FRAME_WIDTH, FRAME_HEIGHT, DVDisplay::MODE_PALETTE);
   display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, DVDisplay::MODE_PALETTE, FRAME_WIDTH, FRAME_HEIGHT);
   
   DVsetup();
   printf("Done!\n");

   graphics.set_font("bitmap8");

   while(true) 
   {
      graphics.set_pen(BLACK);
      graphics.clear();
      display.flip();
      graphics.clear();
      display.flip();
      
      fr = f_mount(&fs, "", 1);
      if (fr != FR_OK) 
      {
         graphics.set_pen(7);
         graphics.text("Failed to mount SD card, error", Point(40, 32+8), FRAME_WIDTH);
         printf("Failed to mount SD card, error: %d\n", fr);
         display.flip();
         sleep_ms(5000000);
      return 0;
      }

      for(f = 0; f<8 ; f++)
      {
         logooffset = f>>1;
         for (y = 0; y < 40; y++ )
         {
            if (y > 9)
            {
               graphics.set_pen(10);
            } else {
               graphics.set_pen(12);
            }
            for (x = 0; x<32 ; x++ )
            {
               b= ( pilogo[y] >> x ) & 1;
               if ( b == 1)
               {
                  graphics.pixel(Point(x + pilogos[logooffset], y));
               //graphics.text(".", Point(x, y), FRAME_WIDTH);
               }
            }
            graphics.set_pen(7);
            graphics.text("+", Point(78, 12), FRAME_WIDTH);
         }
         display.flip();
      }

      sleep_ms(6000);
         
         graphics.set_pen(7);
         graphics.text("files", Point(0, 60), FRAME_WIDTH);
         display.flip();
         graphics.text("files", Point(0, 60), FRAME_WIDTH);
         display.flip();
         
      int pos =0;
      FILINFO file;
      auto dir = new DIR();
      printf("Listing /\n");
      f_opendir(dir, "/");
      while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
         sprintf(midifiles[pos*30],"%s\n",file.fname);
         pos++;
      }
      
      // ################################### #### 
      int selection =0 ;
      int ybutton =1; 
      //while(false)
      //while ( true )
      while ( gpio_get(BUTTON_Y ) )
      {
         if(display.is_button_a_pressed())
         {
            selection --;
            if (selection <0 )
            {
               selection = 4 ;
            }
            while(display.is_button_a_pressed())
            {}
         }
         
         if(display.is_button_x_pressed())
         {
            selection ++;
            if (selection >4 )
            {
               selection = 0 ;
            }
            while(display.is_button_x_pressed())
            {}
         }

         for ( l = 0; l<2; l++)
         {
            for(f = 0; f < 5; f++) {
               if (f == selection) 
               {
                  graphics.set_pen(1);
               } else {
                  graphics.set_pen(0);
               }
               graphics.rectangle({238,f*20+76,200,20});
               graphics.set_pen(7);            
               graphics.text(midifiles[f*30], Point(240, f*20+76), FRAME_WIDTH);
            }  
            display.flip();
         }
         
         graphics.text("....", Point(440, 20), FRAME_WIDTH);
         display.flip();
         graphics.text("....", Point(440, 20), FRAME_WIDTH);
         display.flip();
         
      }
      
      fr = f_open(&audio_file, midifiles[0], FA_READ);
      if (fr != FR_OK) {
         printf("Failed to open midi file, error: %d\n", fr);
         return 0;
      }      
      
      
      char filetext[20];
      
      fill_file_buffer();
      
      sprintf(filetext,"%u",write_buf);
      graphics.text(filetext, Point(20, 180), FRAME_WIDTH);
      display.flip();  
      graphics.text(filetext, Point(20, 180), FRAME_WIDTH);
      display.flip(); 
      
      
      
// SD card read file not working yet

      for (int file =0; file < 15; file ++)
      {
         sprintf(filetext,"%d",file);
         graphics.text("test", Point(320, 180), FRAME_WIDTH);
         graphics.text(filetext, Point(20, file * 16+200), FRAME_WIDTH);
         display.flip();  
         graphics.text("test", Point(320, 180), FRAME_WIDTH);
         graphics.text(filetext, Point(20, file * 16+200), FRAME_WIDTH);
         display.flip();  
         sleep_ms(500);
         graphics.text("test", Point(320, 180), FRAME_WIDTH);
      }
      //sleep_ms(500000);
      

      while(true)
      {
         for ( x =0 ; x <16 ; x ++)
         {
            for ( y =0 ; y <16 ; y ++)
            {
               graphics.set_pen(x);
               graphics.rectangle({x*40,y*32,40,32});
               graphics.set_pen(y);
               graphics.text("AHOY", Point(x*40, y*32+8), FRAME_WIDTH);
               
            }
         }
         display.flip();
         sleep_ms(5000000);
         for (int x =0 ; x <16 ; x ++)
         {
            for (int y =0 ; y <16 ; y ++)
            {
               graphics.set_pen(x);
               graphics.rectangle({x*40,y*32,40,32});
               graphics.set_pen(y);
               graphics.text("AHOY", Point(x*40, y*32+8), FRAME_WIDTH);
            }
         }
         display.flip();
         sleep_ms(5000000);
      }
   }
}
