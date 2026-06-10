// ME433 HW17 pico code: read AS5600 angle and HX711 force, print as CSV

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// ---------------- AS5600 encoder ----------------
#define AS5600_ADDR 0x36
#define AS5600_RAW_ANGLE 0x0C   // high byte of 12-bit raw angle

// read the 12-bit raw angle (0..4095) over I2C
int as5600_read_angle(void) {
    unsigned char reg = AS5600_RAW_ANGLE;
    unsigned char buf[2];
    i2c_write_blocking(i2c_default, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, AS5600_ADDR, buf, 2, false);
    int angle = ((buf[0] << 8) | buf[1]) & 0x0FFF;  // 12 bits
    return angle;
}

// basically copied from hw14
#define SCK_PIN 14
#define DT_PIN  15

void hx711_init(void) {
    gpio_init(SCK_PIN);
    gpio_set_dir(SCK_PIN, GPIO_OUT);
    gpio_put(SCK_PIN, 0);
    gpio_init(DT_PIN);
    gpio_set_dir(DT_PIN, GPIO_IN);
}

int hx711_read(void) {
    // wait for data ready (DT goes low), with timeout to avoid lockup
    int timeout = 100000;
    while (gpio_get(DT_PIN) == 1 && timeout > 0) { timeout--; }

    uint32_t raw = 0; // read 24 bits, MSB first
    for (int i = 0; i < 24; i++) {
        gpio_put(SCK_PIN, 1);
        sleep_us(1);
        raw = (raw << 1) | gpio_get(DT_PIN);
        gpio_put(SCK_PIN, 0);
        sleep_us(1);
    }
    // 25th pulse to set gain for next reading
    gpio_put(SCK_PIN, 1);
    sleep_us(1);
    gpio_put(SCK_PIN, 0);
    sleep_us(1);

    if (raw & 0x800000) raw |= 0xFF000000;  // sign extend
    return (int)raw;
}

int main() {
    stdio_init_all();

    // I2C for the ecncoder 
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    hx711_init();

    // iir filter
    float force_filt = (float)hx711_read();

    while (true) {
        int angle = as5600_read_angle();      // 0..4095
        int force_raw = hx711_read();
        force_filt = 0.85f * force_filt + 0.15f * (float)force_raw;

        printf("%d,%.0f\n", angle, force_filt);  // "angle,force"
        sleep_ms(20);   // 50 Hz
    }
}