// ME433 HW5 - MPU6050 IMU + SSD1306 OLED
// Reads the accelerometer and draws a line from screen center in the
// also requires ssd1306 and font from hw4
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// MPU6050
// Stemedu board ties the address to 0x68. .
#define IMU_ADDR 0x68

#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B   // first of 14 sequential data bytes
#define WHO_AM_I     0x75
// got rid of heartbeat pin after done debugging, but could define here. code was commented out.

// Write one byte to an IMU register.
void imu_write(unsigned char reg, unsigned char val) {
    unsigned char buf[2] = {reg, val};
    i2c_write_blocking(i2c_default, IMU_ADDR, buf, 2, false);
}

// Read one byte from an IMU register.
unsigned char imu_read_byte(unsigned char reg) {
    unsigned char v = 0;
    i2c_write_blocking(i2c_default, IMU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, IMU_ADDR, &v, 1, false);
    return v;
}

void imu_init() {
    imu_write(PWR_MGMT_1, 0x00);    // wake the chip up
    imu_write(ACCEL_CONFIG, 0x00);  // accel +/- 2g
    imu_write(GYRO_CONFIG, 0x18);   // gyro +/- 2000 dps
}

// Burst-read all 14 data bytes (accel XYZ, temp, gyro XYZ) at once.
void imu_read_all(signed short *data) {
    unsigned char reg = ACCEL_XOUT_H;
    unsigned char raw[14];
    i2c_write_blocking(i2c_default, IMU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, IMU_ADDR, raw, 14, false);
    // recombine high/low bytes into 7 signed 16-bit values
    for (int i = 0; i < 7; i++) {
        data[i] = (signed short)((raw[i * 2] << 8) | raw[i * 2 + 1]);
    }
}

// Bresenham line for the OLED.
void drawLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (1) {
        if (x0 >= 0 && x0 < 128 && y0 >= 0 && y0 < 32) ssd1306_drawPixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

int main() {
    stdio_init_all();

    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // gpio_init(HEARTBEAT_PIN);
    // gpio_set_dir(HEARTBEAT_PIN, GPIO_OUT);

    ssd1306_setup();
    imu_init();

    // Sanity check: verify WHO_AM_I before trusting anything else.
    unsigned char who = imu_read_byte(WHO_AM_I);
    if (who != 0x68 && who != 0x98) {
        // wrong/no chip -> light LED solid and stop so you know to power cycle
        // gpio_put(HEARTBEAT_PIN, 1);
        while (1) { tight_loop_contents(); }
    }

    signed short d[7];
   // bool heartbeat = false;
    //unsigned int last_blink = to_us_since_boot(get_absolute_time());

    while (true) {
        //unsigned int now = to_us_since_boot(get_absolute_time());
        //if (now - last_blink >= 500000) {
           // heartbeat = !heartbeat;
           // gpio_put(HEARTBEAT_PIN, heartbeat);
           // last_blink = now;
        //}

        imu_read_all(d);
        // d[0]=accelX  d[1]=accelY  d[2]=accelZ
        float ax = d[0] * 0.000061f;   // -> g
        float ay = d[1] * 0.000061f;

        int cx = 64, cy = 16;          // screen center
        int scale = 20;                // pixels per g
        int ex = cx + (int)(ax * scale);
        int ey = cy + (int)(ay * scale);

        ssd1306_clear();
        drawLine(cx, cy, ex, ey);
        ssd1306_update();
        sleep_ms(10);                  // 100 Hz
    }
}