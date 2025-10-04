//inlcusions:
#include "dtekv-lib.h"
#include "led_ws218.h"
#include <stdlib.h>
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
void snake_init();

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
int snake_len;
pos berry;
char button;

//how to define colors:  (the struct is defined in led_ws218.h file) 
color white= {1,1,1};
color black={0,0,0};
color snake_color={0,1,0};
color berry_color={1,0,0};

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
    pos head = snake[0];
    //figure out new head position
    switch (button){
        case 'u':
            head.z++;
            if(head.z > 4)
                head.z = 0;
            break;
        case 'd':
            head.z--;
            if(head.z < 0)
                head.z = 4;
            break;

        case 'r':
            head.y++;
            if(head.y > 4)
                head.y = 0;
            break;
        case 'l':
            head.y--;
            if(head.y < 0)
                head.y = 4;
            break;

        case 'f':
            head.x++;
            if(head.x > 4)
                head.x = 0;
            break;
        case 'b':
            head.x--;
            if(head.x < 0)
                head.x = 4;
            break;
    };
    //we can make it the color now, as it doesnt matter, it updates when timeout
    cube[head.x][head.y][head.z] = snake_color;
    //make it just longer,we can shorten it down after we check for berry
    for(int i = snake_len; i > 0; i--)
        snake[i] = snake[i-1];
    snake[0] = head;
    
    if((head.x == berry.x)&&(head.y == berry.y)&&(head.z == berry.z)){
        spawn_berry();  //create a new one I assume this lights it up
        snake_len++;   //increase snake length
    }
    else{
        cube[snake[snake_len].x][snake[snake_len].y][snake[snake_len].z] = black;  //turn of the led for tail
        snake[snake_len] = (pos){0,0,0};  //zero it to be explicit
    }
    return;
}
void spawn_berry(){
    bool cross_out[125];
    for(int i = 0; i < snake_len; i++)
        cross_out[snake[i].x*25+5*snake[i].y]+[snake[i].z] = 1;

    int random_number = rand() % (125-snake_len);
    int p = 0;
    for(int i = 0; i < 125; i++)
        p += 1-cross_out[i];

    berry.x = p/25;
    p %= 25;
    berry.y = p/5;
    p %= 25;
    berry.z = p;

    cube[berry.x][berry.y][berry.z] = berry_color;
    
    return;
}
char snake_check(){
    volatile char a = 1;
    return a;
}
void init(){
    init_led();
    snake_init();
    timer_init();
    enable_interrupts();
    return;
}

void snake_init(){
    //head is at 0, for easier code later
    snake_len = 3;
    snake[2] = (pos){0, 0, 0};
    snake[1] = (pos){0, 1, 0};
    snake[0] = (pos){0, 2, 0};
    //already light the leds so it cna stand for a sec
    cube[0][0][0] = snake_color;
    cube[0][1][0] = snake_color;
    cube[0][2][0] = snake_color;
}

void timer_init(){
  volatile int* TMR1_PLow = (volatile int*) 0x04000028;
  volatile int* TMR1_PHigh = (volatile int*) 0x0400002C;
  int timeout = (30000000 / FPS) - 1;
  TMR1_PLow[0] = timeout & 0xFFFF;
  TMR1_PHigh[0] = (timeout >> 16) & 0xFFFF;

  volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer
  TMR1_CTRL[0] = 0x5;  //start the timer, generating interruptsa

  //also, initiate randomness here, cant be arsed to do better
  volatile int* TMR1_SNAPLow = (volatile int*) 0x04000030;
  (*TMR1_SNAPLow) = 0;
  srand((*TMR1_SNAPLow));
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