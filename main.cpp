#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "math.h"

// Docs
// https://github.com/raspberrypi/pico-examples/blob/master/adc/hello_adc/hello_adc.c
// https://youtu.be/Au-oc4hxj-c

#define PWM_WRAP 2000 // Sets PWM period, (8 * PWM_WRAP)ns

int main() {
    stdio_init_all();
    gpio_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
    
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
    gpio_set_function(0, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(0);
    pwm_set_enabled(slice_num, true);

    pwm_set_wrap(slice_num, 500);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 250);

    while (true) {
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        float read_voltage = result * conversion_factor;

        const float wait_factor = PWM_WRAP / 3;
        int duty_cycle = read_voltage * wait_factor;

        pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);
        sleep_ms(500);
    }
}
