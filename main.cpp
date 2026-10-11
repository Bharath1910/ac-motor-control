#include <stdio.h>
#include "pico/stdlib.h"

#define TRIAC_OUT 0

int main() {
    gpio_init(TRIAC_OUT);
    gpio_set_dir(TRIAC_OUT, GPIO_OUT);
    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);

    while (true) {
        gpio_put(TRIAC_OUT, true);
        sleep_ms(50);
        gpio_put(TRIAC_OUT, false);
        sleep_ms(50);
    }
}
