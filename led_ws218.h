#include <stdint.h>
void init_led(void);

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color;

void colour_it(color BUFFER[5][5][5]);