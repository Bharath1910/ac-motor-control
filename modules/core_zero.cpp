#include <stdio.h>
#include <stdint.h>
#include "pico/time.h"
#include "hardware/adc.h"

#define TRIAC_OUT 18
#define ZERO_CROSS_WATCH 19
#define MAX_WAIT_TIME 9900

int core0_loop(uint16_t *pWait_time) {
	while (1) {
        printf("C0 waiting for interrutps\n");
        
        const float conversion_factor = 3.3f / (1 << 12);
        const float wait_factor = MAX_WAIT_TIME / 3.3f;
        
        uint16_t result = adc_read();
        float voltage = result * conversion_factor;
        
         
        *pWait_time = voltage * wait_factor;
        printf("Wait time in core0: %d\n", *pWait_time);

        sleep_ms(1);
    };
}