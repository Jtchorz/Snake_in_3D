#include "dtekv-lib.h"
#include "led_ws218.h"
void handle_interrupt(unsigned int cause); 
void update_cube();
void poll_buttons();
void snake_upd();
void spawn_berry();
void init();
char snake_check();

void handle_interrupt(unsigned int cause){
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
    init_all();
    return;
}
char snake_check(){
    return 1;
}

int main(){
    while(snake_check()){
        poll_buttons();
        snake_upd();
    }
}