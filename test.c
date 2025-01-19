#include "rp2350.h"

#define LED 25
#define GPIO_FUNC_SIO 5
#define INPUT_ENABLED_BITS 40u
#define OUTPUT_DISABLED_BITS 80u

void delay(int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 1000; j++) {
      asm volatile("nop");
    }
  }
}

int main() {
  // sio_hw->gpoio_clr = Rt
  SIO->GPIO_OE_CLR = 1ull << LED;
  SIO->GPIO_OUT_CLR = 1ull << LED;

  // this is for the xor stuff in gpio_set_function, i don't understand quite
  // yet PADS_BANK0_GPIO0_IE_BITS: 0x00000040u PADS_BANK0_GPIO0_OD_BITS:
  // 0x00000080u PADS_BANK0->GPIO25 = ((*(PADS_BANK0->GPIO25) ^
  // INPUT_ENABLED_BITS) &
  //                       (INPUT_ENABLED_BITS | OUTPUT_DISABLED_BITS));
  IO_BANK0->GPIO25_CTRL = GPIO_FUNC_SIO;
  SIO->GPIO_OE_SET = (1ull << LED);
  while (1) {
    SIO->GPIO_OUT_SET = (1ull << LED);
    delay(100);
    SIO->GPIO_OUT_CLR = (1ull << LED);
    delay(1000);
  }
  return 0;
}
