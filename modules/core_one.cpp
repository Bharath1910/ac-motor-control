#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/multicore.h"

// Docs
// https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
// https://youtu.be/Au-oc4hxj-c
// https://stackoverflow.com/a/77935151

#define TRIAC_OUT 18

void core1_loop() {
    while (1) {
        uint32_t offset_us = multicore_fifo_pop_blocking();
        uint32_t wait_time = multicore_fifo_pop_blocking();
        
        // The offset is only applied at the very end of the slow start loop
        // Its to prevent any false triggering at the start of the start loop.
        if (wait_time < (9900 / 8)) {
            busy_wait_us(offset_us);
        }
        busy_wait_us(wait_time);

        gpio_put(TRIAC_OUT, true);
        busy_wait_us(50);
        gpio_put(TRIAC_OUT, false);
    }
}