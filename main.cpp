#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "math.h"


#define TRIAC_OUT 2

int main() {
    stdio_init_all();
    gpio_init(TRIAC_OUT);
    gpio_set_dir(TRIAC_OUT, GPIO_OUT);
    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
    
    adc_init();
    printf("ADC Example, measuring GPIO26\n");
    // https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
    
    while (true) {
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        float read_voltage = result * conversion_factor;

        printf("%f V\n", read_voltage);

        const float wait_factor = 33.3f;
        int wait_time = read_voltage * wait_factor;

        gpio_put(TRIAC_OUT, true);
        sleep_ms(wait_time);
        gpio_put(TRIAC_OUT, false);
        sleep_ms(wait_time);
    }
}
