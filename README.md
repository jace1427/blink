# Post-Mortem

Well, I gave it a great try. Unfortunately after following the stack, trying to learn and understand what each asm command did, etc, etc, I realized that I have the Pico 2W! And the onboard LED is not connected to the rp2350 gpio pins on the Pico2W, it's connected to the wifi chip! WL_GPIO0, to be exact. Many lessons learned, but for now my bare metal blinking days are over. I _might_ come back to this to figure out how to interface with the CYW43439, but no promises.

I am glad I did this, it gave me lots of insight into how the pico-sdk actually works, how it relates to the rp2350's specs, how CMAKE works, and how to setup a pico project with nvim.

# Reverse Engineering pico-sdk

The example given in pico-examples blink.c has 2 functions: pico_led_init, pico_set_led.
Below I walk through each, trying to find out what they do.

- [Getting Started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [Pico2W Datasheet](https://datasheets.raspberrypi.com/picow/pico-2-w-datasheet.pdf)
- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [ARM Docs](https://developer.arm.com/documentation/ddi0553/latest/)
- [pico-examples](https://github.com/raspberrypi/pico-examples)
- [pico-sdk](https://github.com/raspberrypi/pico-sdk)

## pico_led_init

### High Level

pico_led_init sends a bit to the pico 6 times.
First, It sets pin 25 output enable to 0.
It then writes a 0 to pin's output
Then it uses a xor to set input enable on, and output disable off
Then it sets all bits to zero exception fsel, which it sets to 5
Then it clears all iso bits
Finally it sets pin 25 output enable to 1.

### Vars and Args

`PICO_DEFAULT_LED_PIN = 25`: only on the pico2_w

`GPIO_IN = 0`: TODO try to find in datasheet why this is

`GPIO_OUT = 1`: TODO try to find in datasheet why this is

`GPIO_FUNC_SIO = 5`: "The SIO GPIO registers control GPIOs which have the SIO function selected (5)"

`&pads_bank0_hw0->io[gpio]`: the pads_bank0_hw_t is a struct that maps to a location in memory

`PADS_BANK0_GPIO0_IE_BITS`: 0x00000040u

`PADS_BANK0_GPIO0_OD_BITS`: 0x00000080u

`REG_ALIAS_XOR_BITS`: 0x2u << 12u

`hw_alias_check_addr()`: `((uintptr_t)(addr))` - simply checks that addr is a uintptr_t

`IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB`: ?

`PADS_BANK0_GPIO0_ISO_BITS`: 0x00000100u

`REG_ALIAS_CLR_BITS`: 0x3u << 12u

### Stack Trace

- `gpio_init(PICO_DEFAULT_LED_PIN)`
  - `gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_IN)`
    - `gpioc_bit_oe_put(PICO_DEFAULT_LED_PIN, GPIO_IN)`
      - `pico_default_asm_volatile("mcrr p0, #4, %0, %1, c4" : : "r" (PICO_DEFAULT_LED_PIN), "r" (GPIO_IN));`
  - `gpio_put(PICO_DEFAULT_LED_PIN, 0)`
    - `gpioc_bit_out_put(PICO_DEFAULT_LED_PIN, 0)`
      - `pico_default_asm_volatile ("mcrr p0, #4, %0, %1, c0" : : "r" (PICO_DEFAULT_LED_PIN), "r" (0));`
  - `gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_SIO)`
    - `check_gpio_param(PICO_DEFAULT_LED_PIN)`
      - `invalid_params_if(HARDWARE_GPIO, gpio >= NUM_BANK0_GPIOS)` - skipping for now
      - `invalid_params_if(HARDWARE_GPIO, ((uint32_t)fn << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB) & ~IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS)` - skipping for now
      - `hw_write_masked(&pads_bank0_hw->io[gpio], PADS_BANK0_GPIO0_IE_BITS, PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS)`
        - sets input enable on, output disable off
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

## pico_set_led

### High Level

pico_set_led simply writes a 1 or a 0 (given by `led_on`) to GPIO pin 25.

### Vars and Args

`led_on`: argument, bool

### Stack Trace

- `gpio_put(PICO_DEFAULT_LED_PIN, led_on)`
  - `gpio_put(PICO_DEFAULT_LED_PIN, led_on)`
    - `gpioc_bit_out_put(PICO_DEFAULT_LED_PIN, led_on)`
      - `pico_default_asm_volatile ("mcrr p0, #4, %0, %1, c0" : : "r" (PICO_DEFAULT_LED_PIN), "r" (led_on))`

## MCRR

mcrr is the command used by the pico-sdk to move a bit from a register to the coprocessor

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
