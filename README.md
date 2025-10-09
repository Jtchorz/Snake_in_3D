# Snake_in_3D
Snake on a 3D led cube made from WS218B leds, steered via buttons on gpio.

This is a copy of readme.txt:

The code should be compiled using the make command without any additional arguments. The generated main.bin file should be run using the dtekv-run
command onto the dtek board. 

For this to work you also need the LED cube connected on the 33 gpio pin (zero pin of the second GPIO) and the contoller
plugged in inot the first 8 pins of the board and ground.

If there is need to change from -O3 to -O0 uncomment a few line sin the led_ws218.c file in the singleLed_sendColor() function.