// ME433 HW10 - Pico side
// Streams IMU accelerometer X and Y over USB serial as "ax,ay\n".


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h" // most of code is reused from hw5

#define IMU_ADDR 0x68
#define PWR_MGMT_1   0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG  0x1B
#define ACCEL_XOUT_H 0x3B

void imu_write(unsigned char reg, unsigned char val) {
    unsigned char buf[2] = {reg, val};
    i2c_write_blocking(i2c_default, IMU_ADDR, buf, 2, false);
}

void imu_init(void) {
    imu_write(PWR_MGMT_1, 0x00);
    imu_write(ACCEL_CONFIG, 0x00);
    imu_write(GYRO_CONFIG, 0x18);
}

void imu_read_all(signed short *data) {
    unsigned char reg = ACCEL_XOUT_H;
    unsigned char raw[14];
    i2c_write_blocking(i2c_default, IMU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, IMU_ADDR, raw, 14, false);
    for (int i = 0; i < 7; i++) {
        data[i] = (signed short)((raw[i * 2] << 8) | raw[i * 2 + 1]);
    }
}

int main() {
    stdio_init_all();

    i2c_init(i2c_default, 400 * 1000); // 400 kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    imu_init();

    signed short d[7];
    while (true) {
        imu_read_all(d);
        float ax = d[0] * 0.000061f;   // g
        float ay = d[1] * 0.000061f;
        printf("%.3f,%.3f\n", ax, ay);  // send as "ax,ay"
        sleep_ms(20);                   
    }
}