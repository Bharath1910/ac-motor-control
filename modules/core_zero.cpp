#include <stdio.h>
#include <stdint.h>
#include "pico/time.h"
#include "hardware/adc.h"

#define TRIAC_OUT 18
#define ZERO_CROSS_WATCH 19

// Max Iterations is calculated by
// Formula: max_wait_time_needed_in_seconds / (1/ac_frequency) * 2
// (Frequency is multiplied by 2 because rectified output will have 100Hz instead of 50Hz)
// Example: 10s / (1 / 50) * 2
#define MAX_ITERATIONS 1000

int core0_loop(uint16_t *pDelay_step) {
	while (1) {
        printf("C0 waiting for interrutps\n");
        
        const float conversion_factor = 3.3f / (1 << 12);
        const float iteration_factor = MAX_ITERATIONS / 3.3f;
        
        uint16_t result = adc_read();
        float voltage = result * conversion_factor;
        int iteration_step = voltage * iteration_factor;

        *pDelay_step = 9900 / iteration_step;

        printf("delay step in core0: %dus\n", *pDelay_step);
        sleep_ms(500);
    };
}