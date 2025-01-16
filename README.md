# Reverse Engineering pico-sdk

The example given in pico-examples blink.c has 2 functions: pico_led_init, pico_set_led.
Below I walk through each, trying to find out what they do.

- [Getting Started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [ARM Docs](file:///home/jspidell/Downloads/DDI0553B_y_armv8m_arm.pdf)
- [pico-examples](https://github.com/raspberrypi/pico-examples)
- [pico-sdk](https://github.com/raspberrypi/pico-sdk)

## pico_led_init

`pico_led_init()`
`PICO_DEFAULT_LED_PIN = 25`
only on the pico2_w
`GPIO_IN = 0`
TODO try to find in datasheet why this is
`GPIO_FUNC_SIO = 5`:
"The SIO GPIO registers control GPIOs which have the SIO function selected (5)"

- `gpio_init(PICO_DEFAULT_LED_PIN)`
  - `gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_IN)`
    - `gpioc_bit_oe_put(PICO_DEFAULT_LED_PIN, GPIO_IN)`
      - `pico_default_asm_volatile("mcrr p0, #4, %0, %1, c4" : : "r" (PICO_DEFAULT_LED_PIN), "r" (GPIO_IN));`
        - `p0`: coprocessor 0
        - `#4`: ? opcode - is a 4-bit coprocessor-specific opcode
        - `PICO_DEFAULT_LED_PIN`
        - `GPIO_IN`
        - `c4`: coprocessor register
  - `gpio_put(PICO_DEFAULT_LED_PIN, 0)`
    - `gpio_bit_out_put(PICO_DEFAULT_LED_PIN, 0)`
      - `pico_default_asm_volatile ("mcrr p0, #4, %0, %1, c0" : : "r" (pin), "r" (val));`
  - `gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_SIO)`
- `gpio_set_dir`

## MCRR
