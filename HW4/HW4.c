// ME433 HW4 - SSD1306 OLED display
// Draws text from font.h and shows ADC0 in volts plus the display frame rate.
// needs ssd1306 and font.h files from me433 repo

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

#define ADC0_GPIO 26

// Draw a single 5x8 character with its top-left corner at (x, y).
// is one vertical pixel 
void drawLetter(unsigned char x, unsigned char y, char c) {
    int idx = c - 0x20;            // font table starts at the space char (0x20)
    if (idx < 0) return;           // ignore control chars below space
    for (int col = 0; col < 5; col++) {
        unsigned char colBits = ASCII[idx][col];
        for (int row = 0; row < 8; row++) {
            unsigned char on = (colBits >> row) & 0x01;
            ssd1306_drawPixel(x + col, y + row, on);
        }
    }
}

// Draw a null-terminated string starting at (x, y).
//  add 1 pixel of spacing so that there is 6 px per character
void drawMessage(unsigned char x, unsigned char y, char *m) {
    int i = 0;
    while (m[i] != '\0') {
        drawLetter(x + i * 6, y, m[i]);
        i++;
    }
}

int main() {
    stdio_init_all();
 // i2c setup
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);   // breakout has its own pull-ups too
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    ssd1306_setup();


    // ADC on GP26 --- adc 0
    adc_init();
    adc_gpio_init(ADC0_GPIO);
    adc_select_input(0);

    float fps = 0.0f;                         // shows last loop's rate
    bool heartbeat = false;
    unsigned int last_blink = to_us_since_boot(get_absolute_time());

    while (true) {
        unsigned int start = to_us_since_boot(get_absolute_time());


        // Read ADC0 and convert to volts
        uint16_t raw = adc_read();
        float volts = raw * 3.3f / 4095.0f;

        char line[50];
        ssd1306_clear();

        sprintf(line, "ADC0 = %.3f V", volts);
        drawMessage(0, 0, line);

        sprintf(line, "FPS = %.1f", fps);
        drawMessage(0, 24, line);

        ssd1306_update(); // update disp

        unsigned int end = to_us_since_boot(get_absolute_time());
        unsigned int elapsed = end - start;
        if (elapsed > 0) fps = 1000000.0f / elapsed;
    }
}