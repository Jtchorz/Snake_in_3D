#include "led_ws218.h"

extern void print(char *);
extern void print_dec(unsigned int);  //these are here just for testing

#define BRIGHT 255
#define GPIO_address 0x040000E0 //gpio no1
#define wait 8 //how long to wait with the signal ~.4us because we initialise timer once, I will just double this value for longer wait period
                //11 instead of 12 cuz clock is one cycle more
                //we aim for 8 instructions, so that we have a lot of space for overshoot

void init_timer();

volatile int* TMR1_flag = (volatile int*) 0x04000020;  //for checking if timer did done

volatile int* gpio1_data = (volatile int*) GPIO_address;

volatile uint8_t* flagbit = (volatile uint8_t*) 0x04000020;

//they are written in macros to not mess with timings
#define pin_high() ((*gpio1_data) = ((*gpio1_data) | 0x1))   //this is clobbering other pins, I know
    
#define pin_low() ((*gpio1_data) = ((*gpio1_data) & 0xFFFFFFF0) )  //this is clobbering other pins, I know.

#define flag_reset() ((*flagbit) = 0)

#define wait_250() ({while(!((*flagbit)&1)){}})   // ==while(!(*flagbit));

//how to send one singular bit, this process is very timing dependent

////thi function is IMPORTANT if the timing is off stuff breaks ##################################################################################
//#################################################################################################################################
__attribute__((always_inline)) inline void SendBit(char bit){
    
  
    if(bit){
        pin_low();
        flag_reset();   //this is sure that inside the bits, we are actually in sync, not just seeing, oh the flagh is 1 and the timer is whenever
        wait_250();  //this actually is tripped when the timer flag is set, thanks to setup
        flag_reset(); 
        wait_250();  //this actually is tripped when the timer flag is set, thanks to setup
        flag_reset();

        pin_high(); 

        wait_250();
        flag_reset();
        wait_250();   //do three cycles, so that it is ~0.750us
        flag_reset();                            
        wait_250();

        pin_low();                  //do it first, so that the gpio is set down exactly where it needs, then reset the flag, but the tmr is still running
        
        flag_reset();
        //I will on purpose not wait here, as the next will take 250 at least, propably 500. so just not wait, it will be fine
    }
    else
    {
        
        flag_reset();   //this is sure that inside the bits, we are actually in sync, not just seeing, oh the flagh is 1 and the timer is whenever
        wait_250();  //this actually is tripped when the timer flag is set, thanks to setup
        flag_reset(); 
        wait_250();  //this actually is tripped when the timer flag is set, thanks to setup
        flag_reset();
        pin_low();
        pin_high();  

       // wait_250();  //just one cycle high
                        
        pin_low();
    }
}


void singleLed_sendColor(char Red, char Green,char Blue){

    for (int i = 7; i >= 0; i--) {
        SendBit((Green >> i) & 1);
    }

    for (int i = 7; i >= 0; i--) {
        SendBit((Red >> i) & 1);
    }

    for (int i = 7; i >= 0; i--) {
        SendBit((Blue >> i) & 1);
    }

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

void colour_it(color BUFFER[5][5][5])
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
    

    for (int i = 0; i < 125; i++) {
        singleLed_sendColor(BUFFER2[i][0], BUFFER2[i][1], BUFFER2[i][2]);
    }

    //we can delay as long as we want >50us here, the leds will reset,  but for lowest we did experimentally found 140 clock counts
    volatile int cnt = 0;
    
    while (cnt < 500) //wait enough so it resets reliably
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