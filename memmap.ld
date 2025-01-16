MEMORY
{
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 2048K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
    .text : {
        KEEP(*(.vectors))
        *(.text*)
    } > FLASH

    .bss : {
        *(.bss*)
    } > RAM

    .data : {
        *(.data*)
    } > RAM
}
