#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "math.h"

// Docs
// https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
// https://youtu.be/Au-oc4hxj-c
// https://stackoverflow.com/a/77935151

#define TRIAC_OUT 2
#define ZERO_CROSS_WATCH 3
#define MAX_WAIT_TIME 9900

static char event_str[128];
void gpio_event_string(char *buf, uint32_t events);

void handleZeroCross(uint gpio, uint32_t events) {

    uint32_t msSinceBoot = to_ms_since_boot(get_absolute_time());
    printf("Time: %d\n", msSinceBoot);
    
    gpio_event_string(event_str, events);
    printf("GPIO %d %s\n", gpio, event_str);

    const float conversion_factor = 3.3f / (1 << 12);
    const float wait_factor = MAX_WAIT_TIME / 3.3f;

    uint16_t result = adc_read();
    float voltage = result * conversion_factor;
    uint16_t wait_time = voltage * wait_factor;
    
    printf("voltage: %f", voltage);
    printf("wait time %d\n", wait_time);
    busy_wait_us(wait_time);


    gpio_put(TRIAC_OUT, true);
    busy_wait_us(100);
    gpio_put(TRIAC_OUT, false);
}

int main() {
    stdio_init_all();
    
    gpio_init(TRIAC_OUT);
    gpio_init(ZERO_CROSS_WATCH);

    gpio_set_dir(TRIAC_OUT, GPIO_OUT);
    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    
    gpio_set_irq_enabled_with_callback(
        ZERO_CROSS_WATCH, 
        GPIO_IRQ_EDGE_RISE, 
        true, 
        &handleZeroCross
    );

    while (1) {
        printf("waiting for interrutps\n");
        sleep_ms(500);
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