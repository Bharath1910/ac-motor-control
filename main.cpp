#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "math.h"
#include "pico/multicore.h"

// Docs
// https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
// https://youtu.be/Au-oc4hxj-c
// https://stackoverflow.com/a/77935151

#define TRIAC_OUT 18
#define ZERO_CROSS_WATCH 19
#define MAX_WAIT_TIME 9900

static char event_str[128];
uint16_t wait_time = 0;

void gpio_event_string(char *buf, uint32_t events);

void handleZeroCross(uint gpio, uint32_t events) {
    multicore_fifo_push_blocking(wait_time);
}

void core1_loop() {
    while (1) {
        uint32_t wait_time = multicore_fifo_pop_blocking();

        busy_wait_us(wait_time);

        gpio_put(TRIAC_OUT, true);
        busy_wait_us(50);
        gpio_put(TRIAC_OUT, false);
    }
}

int main() {
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
        &handleZeroCross
    );

    multicore_launch_core1(core1_loop);

    while (1) {
        printf("C0 waiting for interrutps\n");
        
        const float conversion_factor = 3.3f / (1 << 12);
        const float wait_factor = MAX_WAIT_TIME / 3.3f;
        
        uint16_t result = adc_read();
        float voltage = result * conversion_factor;
        wait_time = voltage * wait_factor;

        sleep_ms(1);
    };
}

static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}