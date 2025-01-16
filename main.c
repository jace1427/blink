#include <stdbool.h>
#include <sys/cdefs.h>
#include <sys/types.h>

// reset base at 0x40020000
// IO_BANK0 bit 6
//
//

#define LED_DELAY_MS
#define PICO_DEFAULT_LED_PIN = 25;
#define GPIO_IN = 0;
#define pico_default_asm_volatile(...)                                         \
  __asm volatile(".syntax unified\n" __VA_ARGS__)

// Write a 1-bit value to any output enable. Equivalent to:
//
//     if (val)
//         gpioc_hilo_oe_set(1ull << pin);
//     else
//         gpioc_hilo_oe_clr(1ull << pin);
inline __always_inline static void gpioc_bit_oe_put(uint pin, bool val) {
  // TODO inline vs no inline?
  pico_default_asm_volatile("mcrr p0, #4, %0, %1, c4" : : "r"(pin), "r"(val));
}

void pico_led_init() { gpioc_bit_oe_put(PICO_DEFAULT_LED_PIN, GPIO_IN); }

void pico_set_led(bool led_on) {}

int main() {
  pico_led_init();
  while (true) {
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
  }
}
