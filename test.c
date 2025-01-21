#include "pico/cyw43_arch.h"
#include <boards/pico2_w.h>
#include <pico/stdio.h>
#include <pico/time.h>

int main() {
  cyw43_arch_init();
  cyw43_arch_gpio_put(0, 1);

  while (1) {
    cyw43_arch_gpio_put(0, 0);
    sleep_ms(1000);
    cyw43_arch_gpio_put(0, 1);
    sleep_ms(250);
  }
}
