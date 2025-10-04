//inlcusions:
#include "dtekv-lib.h"
#include "led_ws218.h"
#define FPS 1
#define NUM_LEDS 10
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

//global variables:
int timeoutcount = 0;
uint8_t strip[NUM_LEDS][3];


//functions:

void handle_interrupt(unsigned int cause){
    volatile int* TMR1_flag = (volatile int*) 0x04000020;
    volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer

    timeoutcount++;
    colour_it(strip, NUM_LEDS);  //this should return the timer exactly the same

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
char snake_check(){
    volatile char a = 1;
    return a;
}

int main(){
    init();

    while(snake_check()){
        print("iam");
        strip[timeoutcount][0] = 1;
        strip[timeoutcount][1] = 1;
        strip[timeoutcount][2] = 1;
        poll_buttons();
        snake_upd();
    }
}