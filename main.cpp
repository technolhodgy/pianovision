#include "pico/stdlib.h"
#include <cstdio>
#include <iterator>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include "hardware/gpio.h"
//#include "hardware/uart.h"
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


char midifiles[10][30];
char text[30];

unsigned char midibyte[2];


#define NUM_BUFFERS 1
#define BUFFER_LEN 65536
#define BUFFER_BYTES (BUFFER_LEN*2)
unsigned char  buf[BUFFER_LEN];
unsigned char  mididata[BUFFER_LEN];
unsigned int midilenght;
volatile uint write_buf;
volatile uint read_buf;
uint buf_idx;

int f,x,y,l,p;
int b =0;
int retval;
uint  pos =0;
uint  br;

DVDisplay display;
PicoGraphics_PenDV_P5 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

#include"setup.cpp"

//Pen BLACK = graphics.create_pen(0, 0, 0);
//Pen WHITE  = graphics.create_pen(255, 255, 255);
void display_error (char message[30],int status,int flip)
{
   graphics.set_pen(status);            
   graphics.rectangle({330,8,300,20});
   graphics.set_pen(6);            
   graphics.text(text, Point(340, 10), FRAME_WIDTH);
   display.flip();
   if (flip){
      graphics.set_pen(status);            
      graphics.rectangle({330,8,300,20});
      graphics.set_pen(6);            
      graphics.text(text, Point(340, 10), FRAME_WIDTH);
      display.flip();  
   }
}



static uint fill_midi_buffer(char midifile[30]) {
    fr = f_open(&fil, midifile, FA_READ);
        if (fr != FR_OK) {
            printf("Failed to open file, error: %d\n", fr);
            sprintf(text,"Failed to open file, error: %d\n", fr);
            display_error(text,2,1);
            return 0;
        }
        


    uint next_buf_idx = (write_buf + 1) & 0xF;
    if (next_buf_idx == read_buf) 
    {
         sprintf(text,"#########: %d\n", fr);
         display_error(text,2,1);

       return 0;
    }
    
    // #######################################################
    pos=0;
    uint bytes_read;
    while (!f_eof(&fil))
    {
         fr = f_read(&fil, &buf[write_buf], 1, &bytes_read);
         sprintf(text,"%0d",buf[write_buf]);
         mididata[pos] = atoi(text);
         //display_error(text,1,0);
         //sleep_ms(1000);
         pos++;
        
    }
   midilenght = pos;
   /*
    for (f =0; f<pos;f++)
    {
         //sprintf(text,"x%02X %u",buf[f],f);
         sprintf(text,"x%02X %u",mididata[f],f);
         display_error(text,1,0);
         sleep_ms(1000);
    }
         sprintf(text,"%d\n", pos);
         display_error(text,1,1);
    */

    /*pos = 0;
    for (;;) {
       sprintf(text,"%d\n", pos);
         display_error(text,0);
        fr = f_read(&fil, buf[write_buf], 32768, &pos);     // Read a chunk of src file 
        pos++;
        if (fr || pos == 0) break; // error or eof 
    }*/
    //fr = f_read(&fil, buf[write_buf], BUFFER_LEN, &bytes_read);
      
    if (fr != FR_OK) {
         sprintf(text,"Failed to read data, error: %d\n", fr);
         display_error(text,2,1);
        return -1;
    }
    write_buf = next_buf_idx;
    
         sprintf(text,"File loaded: %d\n", fr);
         display_error(text,4,1);

         f_close(&fil);
    return {pos};
}


int main() 
{
   stdio_init_all();

   constexpr uint BUTTON_TX = 0;
   gpio_init(BUTTON_TX);
   gpio_set_dir(BUTTON_TX, GPIO_IN);
   gpio_pull_up(BUTTON_TX);
   constexpr uint BUTTON_RX = 1;
   gpio_init(BUTTON_RX);
   gpio_set_dir(BUTTON_RX, GPIO_IN);
   gpio_pull_up(BUTTON_RX);

   constexpr uint BUTTON_BRX = 4;
   gpio_init(BUTTON_BRX);
   gpio_set_dir(BUTTON_BRX, GPIO_IN);
   gpio_pull_up(BUTTON_BRX);
   constexpr uint BUTTON_BTX = 5;
   gpio_init(BUTTON_BTX);
   gpio_set_dir(BUTTON_BTX, GPIO_IN);
   gpio_pull_up(BUTTON_BTX);
   constexpr uint BUTTON_SDA = 6;

   
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

      sleep_ms(4000);
         
         graphics.set_pen(7);
         graphics.text("files", Point(0, 60), FRAME_WIDTH);
         display.flip();
         graphics.text("files", Point(0, 60), FRAME_WIDTH);
         display.flip();
         

      FILINFO file;
      auto dir = new DIR();
      printf("Listing /\n");
      f_opendir(dir, "/");
      while(f_readdir(dir, &file) == FR_OK && file.fname[0] && pos < 30) {
         //sprintf(midifiles[pos],"%s\n",file.fname);
         strcpy (midifiles[pos],file.fname);
         //midifiles[pos+strlen(file.fname)] = '\0';
         pos++;
      }
      /*for ( pos =0; pos <5;pos ++)
      {

         graphics.set_pen(7);            
         graphics.text(midifiles[pos], Point(40, pos*20+76), FRAME_WIDTH);
         display.flip();
         graphics.text(midifiles[pos], Point(40, pos*20+76), FRAME_WIDTH);
         display.flip();
      }  */    
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
                  if (!gpio_get(BUTTON_RX)) {
                  graphics.set_pen(4);                     
                  }
                  if (!gpio_get(BUTTON_TX)) {
                  graphics.set_pen(6);                     
                  }
                  if (!gpio_get(BUTTON_BRX)) {
                  graphics.set_pen(5);                     
                  }
                  if (!gpio_get(BUTTON_BTX)) {
                  graphics.set_pen(3);                     
                  }

               } else {
                  graphics.set_pen(0);
               }
               graphics.rectangle({236,f*20+74,320,20});
               graphics.set_pen(7);       
               graphics.text(midifiles[f], Point(240, f*20+76), FRAME_WIDTH);
            }  
            display.flip();
         }
         
        /* graphics.text("....", Point(440, 20), FRAME_WIDTH);
         display.flip();
         graphics.text("....", Point(440, 20), FRAME_WIDTH);
         display.flip();
         */
      }
      
      for (p=0;p<2;p++)
      {
         graphics.set_pen(BLACK);
         graphics.clear();
         graphics.set_pen(7);       
         graphics.text("File: ", Point(10, 10), FRAME_WIDTH);
         graphics.set_pen(16);       
         graphics.text(midifiles[selection], Point(60, 10), FRAME_WIDTH);
         display.flip();
      }
      fr = f_open(&audio_file, midifiles[0], FA_READ);
      if (fr != FR_OK) {
         printf("Failed to open midi file, error: %d\n", fr);
         return 0;
      }      
      
      
      char filetext[20];
      
      retval =fill_midi_buffer(midifiles[selection]);
      sprintf(filetext,"%d",retval);
      graphics.set_pen(7);
      graphics.text(filetext, Point(270, 10), FRAME_WIDTH);
      display.flip();  
      graphics.text(filetext, Point(270, 10), FRAME_WIDTH);
      display.flip(); 
      
      
      
// SD card read file not working yet

      for (int file =0; file < 400 ; file ++)
      {
         //sprintf(filetext,"%d",file);
         for (f=0;f<2;f++)
         {
            //strcpy(midibyte[0],buf[file]);
            sprintf(filetext,"x%02X",mididata[file]);
            //graphics.text("test", Point(320, 50), FRAME_WIDTH);
            x = file % 16 ;
            y = (file -x);
            graphics.text(filetext, Point(x*36+64, y+50), FRAME_WIDTH);
            display.flip();  
         }
         sleep_ms(5);
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
