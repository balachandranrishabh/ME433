#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define SERVO_PIN  17
#define SERVO_FREQ 50

static uint     servo_slice;
static uint     servo_chan;
static uint     servo_wrap;

void servo_init(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    servo_slice = pwm_gpio_to_slice_num(gpio);
    servo_chan  = pwm_gpio_to_channel(gpio);

    uint32_t f_sys = clock_get_hz(clk_sys);
    float divider = (float)f_sys / (SERVO_FREQ * 65536.0f);
    if (divider < 1.0f) divider = 1.0f;
    pwm_set_clkdiv(servo_slice, divider);

    servo_wrap = (uint32_t)((float)f_sys / (divider * SERVO_FREQ)) - 1;
    pwm_set_wrap(servo_slice, servo_wrap);

    pwm_set_enabled(servo_slice, true);
}

void servo_set_angle(float angle) {
    if (angle < 0.0f)   angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f; 
    float duty     = pulse_ms / 20.0f;          
    pwm_set_chan_level(servo_slice, servo_chan, (uint16_t)((servo_wrap + 1) * duty));
}

int main() {
    stdio_init_all();
    servo_init(SERVO_PIN);

    while (true) {
        for (float a = 0; a <= 180; a += 2) {
            servo_set_angle(a);
            sleep_ms(20);
        }
        for (float a = 180; a >= 0; a -= 2) {
            servo_set_angle(a);
            sleep_ms(20);
        }
    }
}