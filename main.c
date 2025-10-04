//inlcusions:
#include "dtekv-lib.h"
#include "led_ws218.h"
#define FPS 1
#define NUM_LEDS 125
extern void enable_interrupts( void );

//implicit declarations:
void handle_interrupt(unsigned int cause); 
void update_cube();
void poll_buttons();
void snake_upd();
void spawn_berry();
void init();
void timer_init();
char snake_check();

//structs:
typedef struct {
    int x;
    int y;
    int z;
} pos;

//global variables:
volatile int timeoutcount = 0;
color cube[5][5][5];
pos snake[125];
pos berry;

//how to define colors:  (the struct is defined in led_ws218.h file) 
color white = {1,1,1};
color black={0,0,0};
color green={0,1,0};

//functions:

void handle_interrupt(unsigned int cause){
    volatile int* TMR1_flag = (volatile int*) 0x04000020;
    volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer

    timeoutcount++;
    colour_it(cube);  //this should return the timer exactly the same

    TMR1_flag[0] = 0;
    TMR1_CTRL[0] = 0x5;  //start the timer back up again
    return;
    
}

void update_cube(){
    return;
}
void poll_buttons(){
    return;
}
void snake_upd(){
    return;
}
void spawn_berry(){
    return;
}
char snake_check(){
    volatile char a = 1;
    return a;
}
void init(){
    init_led();
    timer_init();
    enable_interrupts();
    return;
}

void timer_init(){
  volatile int* TMR1_PLow = (volatile int*) 0x04000028;
  volatile int* TMR1_PHigh = (volatile int*) 0x0400002C;
  int timeout = (30000000 / FPS) - 1;
  TMR1_PLow[0] = timeout & 0xFFFF;
  TMR1_PHigh[0] = (timeout >> 16) & 0xFFFF;

  volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer
  TMR1_CTRL[0] = 0x5;  //start the timer, generating interrupts
  return;
}

int main(){
    init();
    while(snake_check()){
       /* if(timeoutcount >= 5)
            timeoutcount = 0;

        cube[0][0][timeoutcount] = green;  //so simple to do colors
        */
        poll_buttons();

        if(timeoutcount == 24)
            snake_upd();
    }
}