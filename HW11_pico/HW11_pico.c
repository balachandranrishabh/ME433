// ME433 HW11 - Pico side UART echo
// Bridges the computer (USB serial) and the STM32 (UART0).

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID   uart0
#define BAUD      115200 // baud rate
#define UART_TX   0     
#define UART_RX   1     

int main() {
    stdio_init_all();

    uart_init(UART_ID, BAUD);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);

    while (true) {
        // computer -> STM32: if a char came in over USB, send it out the UART
        int c = getchar_timeout_us(0);   // non-blocking
        if (c != PICO_ERROR_TIMEOUT) {
            uart_putc_raw(UART_ID, (char)c);
        }
 
        // STM32 -> computer: if a char came in on the UART, print it to USB
        if (uart_is_readable(UART_ID)) {
            char ch = uart_getc(UART_ID);
            putchar(ch);
            fflush(stdout);

        }
    }
}