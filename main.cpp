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
#define GREEN_LED 20
#define YELLOW_LED 21 

uint16_t wait_time = 9800;
uint16_t delay_step = 500;
uint16_t offset_time_us = 0;

void zero_cross_callback(uint gpio, uint32_t events) {
    multicore_fifo_push_blocking(delay_step);
}

void startup() {
    stdio_init_all();
    
    gpio_init(TRIAC_OUT);
    gpio_init(ZERO_CROSS_WATCH);
    gpio_init(GREEN_LED);
    gpio_init(YELLOW_LED);

    gpio_set_dir(TRIAC_OUT, GPIO_OUT);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);

    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);

    adc_init();
    adc_gpio_init(27);
    adc_gpio_init(26);

    gpio_put(GREEN_LED, true);
    gpio_put(YELLOW_LED, true);
    sleep_ms(2000);
    gpio_put(GREEN_LED, false);
    
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
    core0_loop(&delay_step, &offset_time_us);
}