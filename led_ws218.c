//this file was written by Jan Tchorzewski it was exaustive work and is very fragile
#include "led_ws218.h"

extern void print(char *);
extern void print_dec(unsigned int);  //these are here just for testing

#define BRIGHT 255
#define GPIO_address 0x040000F0 //gpio no2
#define wait 10 //how long to wait with the signal ~.4us because we initialise timer once, I will just double this value for longer wait period
                //11 instead of 12 cuz clock is one cycle more
                //we aim for 8 instructions, so that we have a lot of space for overshoot

void init_timer();

volatile int* TMR1_flag = (volatile int*) 0x04000020;  //for checking if timer did done

volatile int* const gpio1_data = (volatile int*) GPIO_address;

volatile uint8_t* flagbit = (volatile uint8_t*) 0x04000020;
volatile int flag;

//they are written in macros to not mess with timings
#define pin_high() ((*gpio1_data) = 1)
    
#define pin_low() ((*gpio1_data) = 0)

#define flag_reset() ((*flagbit) = 0)

#define wait_250() ({while(!((*flagbit)&1)){;}})   // ==while(!(*flagbit));

int gpio_low;
int gpio_high;
void init_gpio();
void gpio_off();

//how to send one singular bit, this process is very timing dependent

////this function is IMPORTANT if the timing is off stuff breaks  do not modify it, 
// I made it work both at o3 and o0 and it is between god and me
// ##################################################################################
//#################################################################################################################################
/*__attribute__((always_inline)) inline void SendBit(char bit){
    
  
    if(__builtin_expect(bit, 0)){

        flag_reset();   //this is sure that inside the bits, we are actually in sync, not just seeing, oh the flag is 1 and the timer is whenever
        wait_250();  //this actually is tripped when the timer flag is set, thanks to setup
        
        pin_low();

        pin_high();
        flag_reset(); 
        wait_250();
        flag_reset();
        wait_250();
        flag_reset();
        pin_low();   
                       //do it first, so that the gpio is set down exactly where it needs, then reset the flag, but the tmr is still running
        wait_250();   //do three cycles, so that it is ~0.750us
        flag_reset();
        //I will on purpose not wait here, as the next will take 250 at least, propably 500. so just not wait, it will be fine
    }
    else
    {
        
        flag_reset();   //this is to give the processor time for if shenanigans
        wait_250();  
        flag_reset(); 

        pin_low();   //this is to make cache work
        pin_low();
        pin_low();

        pin_high();
        asm volatile("nop");
        pin_low();

        wait_250();
        flag_reset();

    }
}*/
__attribute__((always_inline, aligned(32))) inline void sendone(){

        pin_low();
        flag_reset();   //this is warmup for 
        wait_250(); 
        flag_reset();

        pin_high();

        flag_reset(); 
        wait_250();  //wait around 750ns when the function is already running, the first 
        flag_reset();
        wait_250();
        flag_reset();
        
        pin_low();   
                       //do it first, so that the gpio is set down exactly where it needs, then reset the flag, but the tmr is still running
        wait_250();   //do three cycles, so that it is ~0.750us
        flag_reset();
        wait_250();  //this makes distance between leds
        flag_reset();
        //I will on purpose not wait here, as the next will take 250 at least, propably 500. so just not wait, it will be fine
}

__attribute__((always_inline, aligned(32))) inline void sendzero(){

        pin_low();
       // asm volatile("nop");
        flag_reset();   //this is to give the processor time for if shenanigans otherwise the 
        wait_250();  
        flag_reset(); 
        wait_250();  
        flag_reset(); 
        

        pin_low();   //this is to make cache work

        asm volatile("nop");
        pin_high();
        asm volatile("nop");
        pin_low();

        asm volatile("nop");
        wait_250();
        flag_reset();
        wait_250();  
        flag_reset(); 

        

}

__attribute__((always_inline)) inline void singleLed_sendColor(char Red, char Green,char Blue){

    pin_low();  //warm up cache
    asm volatile("nop");  //prevent reordering

    //remove this  waits for -O0
    wait_250();
    flag_reset();
    wait_250();
    flag_reset();
    pin_low();  //warm up cache
    asm volatile("nop");


    for (int i = 7; i >= 0; i--) {
        if(__builtin_expect(((Green >> i) & 1), 0))
            sendone();
        else{
            sendzero();
        }
        //SendBit((Green >> i) & 1);
    }
    pin_low();
    asm volatile("nop");  //prevent reordering
    for (int i = 7; i >= 0; i--) {
        if(__builtin_expect(((Red >> i) & 1), 0))
            sendone();
        else{
            sendzero();
        }
        //SendBit((Red >> i) & 1);
    }
    
    pin_low();
    asm volatile("nop");  //prevent reordering
    for (int i = 7; i >= 0; i--) {
        if(__builtin_expect(((Blue>> i) & 1), 0))
            sendone();
        else{
            sendzero();
        }
        //SendBit((Blue >> i) & 1);
    }

    wait_250();  //this makes distance between leds
    flag_reset();
    wait_250();  //this makes distance between leds
    flag_reset();
    wait_250();  //this makes distance between leds
    flag_reset();

}

//variables for saving timers, better global, because colour_it doesnt use them.
volatile uint16_t timer[4];

void save_timer(){
    volatile uint16_t* address = (volatile uint16_t*) 0x04000020;

    for(int i = 0; i<4; i++)
    {
        timer[i]=(*address);
        address+=2;
    }
    return;
}

void restore_timer(){
    volatile uint16_t* address = (volatile uint16_t*) 0x04000020;

    for(int i = 0; i<4; i++)
    {
        (*address)=timer[i];
        address+=2;
    }
    return;
}

void init_timer(void)
{
  volatile uint16_t* TMR1_PLow = (volatile uint16_t*) 0x04000028;
  volatile uint16_t* TMR1_PHigh = (volatile uint16_t*) 0x0400002C;
  volatile int* TMR1_CTRL = (volatile int*) 0x04000024; //for starting and stopping timer
  volatile int* TMR1_STAT  = (volatile int*) 0x04000020;
  TMR1_CTRL[0] = 0x8;
  TMR1_STAT[0] = 0x1;

  int timeout = wait;
  TMR1_PLow[0] = timeout & 0xFFFF;
  TMR1_PHigh[0] = (timeout >> 16) & 0xFFFF;


  TMR1_CTRL[0] = 0x6;  //start the timer in continous mode for better accuracy
  return;
}

void colour_it(volatile color BUFFER[5][5][5])
{
    //save previous timeout, and start our own timer
    save_timer();
    init_timer();

    //new buffer because reading from 3d one is way too slow
    char BUFFER2[125][3];

    //convert it to a single table, it isnt fast enough otherwise
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 5; k++){
                if(i%2 == 0){
                BUFFER2[(25*i)+(5*j)+k][0] = BUFFER[i][j][k].red;
                BUFFER2[(25*i)+(5*j)+k][1] = BUFFER[i][j][k].green;
                BUFFER2[(25*i)+(5*j)+k][2] = BUFFER[i][j][k].blue;
                }
                else{
                BUFFER2[(25*i)+(5*j)+k][0] = BUFFER[i][4-j][k].red;
                BUFFER2[(25*i)+(5*j)+k][1] = BUFFER[i][4-j][k].green;
                BUFFER2[(25*i)+(5*j)+k][2] = BUFFER[i][4-j][k].blue;
                }
            }
        }
    }
    gpio_high = ((*gpio1_data) | 0x1);
    gpio_low = ((*gpio1_data) & 0xFFFFFFF0);

   /* for (int i = 0; i < 125; i++) {
        singleLed_sendColor(BUFFER2[i][0], BUFFER2[i][1], BUFFER2[i][2]);
    }
*/
    int i = 0;
    do{
        singleLed_sendColor(BUFFER2[i][0], BUFFER2[i][1], BUFFER2[i][2]);
        i++;
    }while(i<125);
    //we can delay as long as we want >50us here, the leds will reset,  but for lowest we did experimentally found 140 clock counts
    volatile int cnt = 0;
    
    while (cnt < 100) //wait enough so it resets reliably
    {
        wait_250();
        flag_reset();
        cnt++;
    }
    restore_timer();
}

void init_gpio(void)
{
    volatile int* GPIO_dir = (volatile int*) (GPIO_address + 0x4);
    GPIO_dir[0] = GPIO_dir[0] | 0x1; //set it as output 

    pin_low();
    pin_low();

}

void init_led(void)
{
    init_gpio();
    print("initialised \n");
}