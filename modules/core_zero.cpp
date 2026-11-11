#include <stdio.h>
#include <stdint.h>
#include "pico/time.h"
#include "hardware/adc.h"

#define TRIAC_OUT 18
#define ZERO_CROSS_WATCH 19
#define MAX_OFFSET_US 1000
// Max Iterations is calculated by
// Formula: max_wait_time_needed_in_seconds / (1/ac_frequency) * 2
// (Frequency is multiplied by 2 because rectified output will have 100Hz instead of 50Hz)
// Example: 10s / (1 / 50) * 2
#define MAX_ITERATIONS 1000

int core0_loop(uint16_t *p_delay_step, uint16_t *p_offset_us) {
	while (1) {
        printf("C0 waiting for interrutps\n");
        
        const float conversion_factor = 3.3f / (1 << 12);

        const float iteration_factor = MAX_ITERATIONS / 3.3f;
        const float offset_factor = MAX_OFFSET_US / 3.3f;
        
        adc_select_input(1);
        uint16_t timer_result = adc_read();

        adc_select_input(0);
        uint16_t offset_result = adc_read();

        float timer_voltage = timer_result * conversion_factor;
        float offset_voltage = offset_result * conversion_factor;

        int iteration_step = timer_voltage * iteration_factor;

        *p_offset_us = offset_voltage * offset_factor;
        *p_delay_step = 9900 / iteration_step;

        printf("delay step in core0: %dus\n", *p_delay_step);
        printf("offset size in core0: %dus\n", *p_offset_us);
        sleep_ms(500);
    };
}