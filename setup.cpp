void DVsetup (void){
   

#define BLACK 0
#define BLUE 1
#define RED 2
#define MEGENTA 3
#define GREEN 4
#define CYAN 5
#define YELLOW 6
#define WHITE 7
#define brightBLACK 8
#define brightBLUE 9
#define brightRED 10
#define brightMEGENTA 11
#define brightGREEN 12
#define brightCYAN 13
#define brightYELLOW 14
#define brightWHITE 15
#define SKYBLUE 16
#define darkGREY 17

   graphics.create_pen(0, 0, 0);
   graphics.create_pen(0, 0, 0xdd);
   graphics.create_pen(0xdd, 0, 0);
   graphics.create_pen(0xdd, 0, 0xdd);
   graphics.create_pen(0, 0xdd, 0);
   graphics.create_pen(0, 0xdd, 0xdd);
   graphics.create_pen(0xdd, 0xdd, 0);
   graphics.create_pen(0xdd, 0xdd, 0xdd);
   graphics.create_pen(0x44, 0x44, 0x44);
   graphics.create_pen(0, 0, 0xff);
   graphics.create_pen(0xff, 0, 0);
   graphics.create_pen(0xff, 0, 0xff);
   graphics.create_pen(0, 0xff, 0);
   graphics.create_pen(0, 0xff, 0xff);
   graphics.create_pen(0xff, 0xff, 0);
   graphics.create_pen(0xff, 0xff, 0xff);
   graphics.create_pen(0, 0x88, 0xff);
   graphics.create_pen(0x88, 0x88, 0x88);
   
}
