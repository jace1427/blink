# Reverse Engineering pico-sdk

The example given in pico-examples blink.c has 2 functions: pico_led_init, pico_set_led.
Below I walk through each, trying to find out what they do.

- [Getting Started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [ARM Docs](https://developer.arm.com/documentation/ddi0553/latest/)
- [pico-examples](https://github.com/raspberrypi/pico-examples)
- [pico-sdk](https://github.com/raspberrypi/pico-sdk)

## pico_led_init

`PICO_DEFAULT_LED_PIN = 25`: only on the pico2_w

`GPIO_IN = 0`: TODO try to find in datasheet why this is

`GPIO_OUT = 1`: TODO try to find in datasheet why this is

`GPIO_FUNC_SIO = 5`: "The SIO GPIO registers control GPIOs which have the SIO function selected (5)"

- `gpio_init(PICO_DEFAULT_LED_PIN)`
  - `gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_IN)`
    - `gpioc_bit_oe_put(PICO_DEFAULT_LED_PIN, GPIO_IN)`
      - `pico_default_asm_volatile("mcrr p0, #4, %0, %1, c4" : : "r" (PICO_DEFAULT_LED_PIN), "r" (GPIO_IN));`
  - `gpio_put(PICO_DEFAULT_LED_PIN, 0)`
    - `gpio_bit_out_put(PICO_DEFAULT_LED_PIN, 0)`
      - `pico_default_asm_volatile ("mcrr p0, #4, %0, %1, c0" : : "r" (PICO_DEFAULT_LED_PIN), "r" (0));`
  - `gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_SIO)`
    - `check_gpio_param(PICO_DEFAULT_LED_PIN)`
      - `invalid_params_if(HARDWARE_GPIO, gpio >= NUM_BANK0_GPIOS)` - skipping for now
      - `invalid_params_if(HARDWARE_GPIO, ((uint32_t)fn << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB) & ~IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS)` - skipping for now
      - `hw_write_masked(&pads_bank0_hw->io[gpio], PADS_BANK0_GPIO0_IE_BITS, PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS)` - sets input enable on, output disable off
        - `hw_xor_bits(addr, (*addr ^ values) & write_mask)`
          - `*(io_rw_32 *) hw_xor_alias_untyped((volatile void *) addr) = mask;`
            - `#define hw_xor_alias_untyped(addr) ((void *)(REG_ALIAS_XOR_BITS + hw_alias_check_addr(addr)))`
      - `io_bank0_hw->io[gpio].ctrl = fn << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB`
        - Zero all fields apart from fsel; we want this IO to do what the peripheral tells it.
        - This doesn't affect e.g. pullup/pulldown, as these are in pad controls.
      - `hw_clear_bits(&pads_bank0_hw->io[gpio], PADS_BANK0_GPIO0_ISO_BITS)`
        - `*(io_rw_32 *) hw_clear_alias_untyped((volatile void *) addr) = mask`
          - `#define hw_clear_alias_untyped(addr) ((void *)(REG_ALIAS_CLR_BITS + hw_alias_check_addr(addr)))`
- `gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT)`
  - `gpioc_bit_oe_put(PICO_DEFAULT_LED_PIN, GPIO_OUT)`
    - `pico_default_asm_volatile("mcrr p0, #4, %0, %1, c4" : : "r" (PICO_DEFAULT_LED_PIN), "r" (GPIO_OUT);`

## MCRR

`MCRR{cond} coproc, #opcode, Rt, Rt2, CRm`

- coproc: `p0`
  coprocessor 0
- opcode: `#4`
  is a 4-bit coprocessor-specific opcode
- Rt: `PICO_DEFAULT_LED_PIN`
  ARM source registers
- Rt2: `GPIO_IN`
  ARM source registers
- CRm: `c4`
  coprocessor register
