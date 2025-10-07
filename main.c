//inlcusions:
#include "dtekv-lib.h"
#include "led_ws218.h"
#define FPS 24
#define NUM_LEDS 125
extern void enable_interrupts( void );

//implicit declarations:
void handle_interrupt(unsigned int cause); 
void poll_buttons();
void snake_upd();
void spawn_berry();
void init();
void timer_init();
char snake_check();
void snake_init();
int rand();
void gpio_init();

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
char direction;
int seed;

//how to define colors:  (the struct is defined in led_ws218.h file) 
color white= {1,1,1};
color black={0,0,0};
color snake_color={0,1,0};
color berry_color={1,0,0};
color head_color = {0,5,0};

//functions:

void handle_interrupt(unsigned int cause){
    if(cause == 16){
    volatile int* TMR1_flag = (volatile int*) 0x04000020;
    volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer

    timeoutcount++;
    colour_it(cube);  //this should return the timer exactly the same

    TMR1_flag[0] = 0;
    TMR1_CTRL[0] = 0x5;  //start the timer back up again
    }
    else{
        print("unidentified interrupt \n");
    }
        return;
    
}

void poll_buttons(){
    volatile int* gpio1_data = (volatile int*) 0x040000E0;
    int data = *(gpio1_data) & 0xFF;
 //   print_dec(data);
   // print("\n");
    direction = 'f';
    //this has to change smth called dir, if the button pressed is directly opposite to dir, then discard it
    //I put this here, as snake upd does moveent, so it cannot discard, so this will validate, that there is only one button pressed, and that it is
    //one that is valid, not in the exact opposite direction of moving
    return;
}
void snake_upd(){
    pos head = snake[0];
    //figure out new head position
    switch (direction){
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
    cube[head.x][head.y][head.z] = head_color;
    cube[snake[0].x][snake[0].y][snake[0].z] =snake_color;

    //make it just longer,we can shorten it down after we check for berry
    for(int i = snake_len; i > 0; i--)
        snake[i] = snake[i-1];
    snake[0] = head;
    snake_len++;
    for(int i = 0; i <= snake_len; i++){
        print_dec(snake[i].x);
        print(" ");
        print_dec(snake[i].y);
        print(" ");
        print_dec(snake[i].z);
        print("\n");
    }
    print("\n");
    
    if((head.x == berry.x)&&(head.y == berry.y)&&(head.z == berry.z)){
        spawn_berry();  //create a new one I assume this lights it up
           //increase snake length
    }
    else{
        snake_len--;
        cube[snake[snake_len].x][snake[snake_len].y][snake[snake_len].z] = white;  //turn of the led for tail
        snake[snake_len] = (pos){0,0,0};  //zero it to be explicit
        
    }
    return;
}
void spawn_berry(){
    int cross_out[125];
    for(int i = 0; i < 125; i++)
        cross_out[i] = 0;

    for(int i = 0; i < snake_len; i++)
        cross_out[snake[i].x*25+5*snake[i].y+snake[i].z] = 1;

    int random_number = 1 + (rand() % ( 125-snake_len) ) ;  //this is how many zeros we want to see
    int p = 0;   //this is the position of berry after
    while(random_number>0){
        random_number -= (1-cross_out[p]);
        p++;
    }
    p--;   //correct cuz we add always

    print_dec(random_number);
    print(" ");
    print_dec(p);
    print("\n");

    berry.x = p/25;
    p %= 25;
    berry.y = p/5;
    p %= 5;
    berry.z = p;

    cube[berry.x][berry.y][berry.z] = berry_color;
    
    return;
}
char snake_check(){
    for(int i = 1; i < snake_len; i++){
        if((snake[0].x == snake[i].x) && (snake[0].y == snake[i].y) && (snake[0].z == snake[i].z))
            return 0;
    }
    return 1;
}








void init(){
    init_led();
    snake_init();
    timer_init();
    enable_interrupts();
    gpio_init();
    return;
}

void snake_init(){
    //head is at 0, for easier code later
    snake_len = 3;
   // snake[4] = (pos){0, 0, 0};
  //  snake[3] = (pos){0, 1, 0};
    snake[2] = (pos){0, 0, 0};
    snake[1] = (pos){0, 1, 0};
    snake[0] = (pos){0, 2, 0};
    //already light the leds so it cna stand for a sec
    cube[0][0][0] = snake_color;
    cube[0][1][0] = snake_color;
    cube[0][2][0] = head_color;
   // cube[0][3][0] = snake_color;
 //   cube[0][4][0] = head_color;

    berry = (pos){3,4,0};
    cube[berry.x][berry.y][berry.z] = berry_color;

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


void gpio_init(){
    volatile int* gpio1_dir = (volatile int*) 0x040000E4;
    *gpio1_dir = (*gpio1_dir) & 0xFFFFFF81;
    return;   
}


int main(){
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 5; k++){
                cube[i][j][k] = white;
            }
        }
    }
    init();

    //seed randomness in switches
    volatile int* switches = (volatile int*) 0x04000010 ;
    seed = (*switches);
    
    while(1){
        poll_buttons();

        if(timeoutcount >= 12){
            snake_upd();
            timeoutcount = 0;
            if(!snake_check())
                break;
        }
        
    }
}








int rand(){
    seed = seed * 187240 + 94512;
    return seed;
}