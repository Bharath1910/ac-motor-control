#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "pico/multicore.h"
#include "modules/core_one.h"
#include "modules/core_zero.h"

// Docs
// https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
// https://youtu.be/Au-oc4hxj-c
// https://stackoverflow.com/a/77935151

#define TRIAC_OUT 18
#define ZERO_CROSS_WATCH 19
#define MAX_WAIT_TIME 9900

uint16_t wait_time = 0;

void zero_cross_callback(uint gpio, uint32_t events) {
    multicore_fifo_push_blocking(wait_time);
}

void startup() {
    stdio_init_all();
    
    gpio_init(TRIAC_OUT);
    gpio_init(ZERO_CROSS_WATCH);

    gpio_set_dir(TRIAC_OUT, GPIO_OUT);
    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);

    adc_init();
    adc_gpio_init(27);
    adc_select_input(1);
    
    gpio_set_irq_enabled_with_callback(
        ZERO_CROSS_WATCH, 
        GPIO_IRQ_EDGE_RISE, 
        true, 
        &zero_cross_callback
    );

    multicore_launch_core1(core1_loop);
}

int main() {
    startup();
    core0_loop(wait_time);
}