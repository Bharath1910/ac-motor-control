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
#define MAX_DELAY 9900

int core0_loop(uint16_t *p_delay_step, uint16_t *p_offset_us) {
	while (1) {        
        const float conversion_factor = 3.3f / (1 << 12);

        const float delay_factor = MAX_DELAY / 3.3f;
        const float offset_factor = MAX_OFFSET_US / 3.3f;
        
        adc_select_input(1);
        uint16_t timer_result = adc_read();

        adc_select_input(0);
        uint16_t offset_result = adc_read();

        float timer_voltage = timer_result * conversion_factor;
        float offset_voltage = offset_result * conversion_factor;

        int delay_us = timer_voltage * delay_factor;

        *p_offset_us = offset_voltage * offset_factor;
        *p_delay_step = delay_us;

        sleep_ms(10);
    };
}