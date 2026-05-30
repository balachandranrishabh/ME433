// ME433 HW3 - MCP23008 I2C IO expander
// Reads a button on the expander's GP0 and turns on an LED on the
// expander's GP7. The heartbeat LED on the Pico itself toggles to show the program is alive.

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// --- MCP23008 ---
// Address with A0, A1, A2 all tied to GND:
#define MCP_ADDR 0x20

// MCP23008 register addresses (from the datasheet):
#define IODIR 0x00   
#define GPPU  0x06   // pull-up enable 
#define GPIO  0x09   // read pin states here
#define OLAT  0x0A   

#define EXP_LED 7    // LED is on the expander's GP7
#define EXP_BTN 0    // button is on the expander's GP0

// Heartbeat LED on the Pico itself.
// NOTE: this works on a plain Pico/Pico 2. On a Pico 2 *W* the onboard LED is
// on the wireless chip and this won't compile/work -- see notes in chat.
#define HEARTBEAT_PIN PICO_DEFAULT_LED_PIN

// Write one byte to a register on the MCP23008.
void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    unsigned char buf[2];
    buf[0] = reg;     // register to write to
    buf[1] = value;   // value to write
    i2c_write_blocking(i2c_default, address, buf, 2, false);
}

// Read one byte from a register on the MCP23008.
unsigned char readPin(unsigned char address, unsigned char reg) {
    unsigned char value = 0;
    i2c_write_blocking(i2c_default, address, &reg, 1, true);  // true: keep bus
    i2c_read_blocking(i2c_default, address, &value, 1, false);
    return value;
}

int main() {
    stdio_init_all();

    // I2C setup (I2C0 default pins) 
    i2c_init(i2c_default, 400 * 1000); // 400 kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    // Your external 10k resistors do the real pull-up work; these just help.
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Heartbeat LED setup
    gpio_init(HEARTBEAT_PIN);
    gpio_set_dir(HEARTBEAT_PIN, GPIO_OUT);
    setPin(MCP_ADDR, IODIR, 0x7F);

    bool heartbeat = false;
    while (true) {
        // Toggle the heartbeat LED so we can see the Pico is alive.
        heartbeat = !heartbeat;
        gpio_put(HEARTBEAT_PIN, heartbeat);
        unsigned char gpio_state = readPin(MCP_ADDR, GPIO);


        bool pressed = (((gpio_state >> EXP_BTN) & 0x01) == 0);

        if (pressed) {
            setPin(MCP_ADDR, OLAT, (1 << EXP_LED));
        } else {
            setPin(MCP_ADDR, OLAT, 0x00);
        }

        sleep_ms(100); 
    }
}