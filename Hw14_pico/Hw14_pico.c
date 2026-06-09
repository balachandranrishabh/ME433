// ME433 HW14 - HX711 load cell amplifier (bit-banged)


#include <stdio.h>
#include "pico/stdlib.h"

#define SCK_PIN 14
#define DT_PIN  15
#define MAX_SAMPLES 1000
// setting pins
void hx711_init(void) {
    gpio_init(SCK_PIN);
    gpio_set_dir(SCK_PIN, GPIO_OUT);
    gpio_put(SCK_PIN, 0);

    gpio_init(DT_PIN);
    gpio_set_dir(DT_PIN, GPIO_IN);
}

// Read one 24-bit sample, extended to a signed 32-bit int.
int hx711_read(void) {
    // wait until data is ready, this is signaled by DT going low, so wait while it's high
    while (gpio_get(DT_PIN) == 1) {
        tight_loop_contents();
    }

    uint32_t raw = 0;
    for (int i = 0; i < 24; i++) {
        gpio_put(SCK_PIN, 1);
        sleep_us(1);
        raw = (raw << 1) | gpio_get(DT_PIN);   // read after clock high
        gpio_put(SCK_PIN, 0);
        sleep_us(1);
    }

    // 25th pulse -> sets gain = 128 for the next read
    gpio_put(SCK_PIN, 1);
    sleep_us(1);
    gpio_put(SCK_PIN, 0);
    sleep_us(1);

    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }
    return (int)raw;
}

int main() {
    stdio_init_all();
    hx711_init();

    int raw_data[MAX_SAMPLES];
    float filt_data[MAX_SAMPLES];
    uint32_t time_data[MAX_SAMPLES];

    while (true) {
        // wait for the user to enter the number of samples to read
        int n = 0;
        if (scanf("%d", &n) == 1) {
            if (n < 1) n = 1;
            if (n > MAX_SAMPLES) n = MAX_SAMPLES;

            float filtered = (float)hx711_read();   // seed the IIR
            uint32_t t0 = to_ms_since_boot(get_absolute_time());

            for (int i = 0; i < n; i++) {
                int r = hx711_read();
                // IIR low-pass: tune A toward 1.0 for more smoothing
                float A = 0.85f;
                filtered = A * filtered + (1.0f - A) * (float)r;

                raw_data[i]  = r;
                filt_data[i] = filtered;
                time_data[i] = to_ms_since_boot(get_absolute_time()) - t0;
            }

            for (int i = 0; i < n; i++) {
                printf("%lu,%d,%.1f\n", time_data[i], raw_data[i], filt_data[i]);
            }
        }
    }
}