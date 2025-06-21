#ifndef PERIPHERALS
#define PERIPHERALS

#include "../../src/defs.h"

/*
 * SoC: Broadcom BCM2837
 *      SPEC: https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf
 * CPU: Quad Core ARMv8 Cortex-A53
 * GPU: Broadcom VideoCore IV
 *
 * Display IC: https://www.hpinfotech.ro/ILI9486.pdf
 */

/* Set current model as RPI3B */
#define RPI3

#if defined (RPI0) || defined (RPI1)
    /* SEE: https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
    *      ^ page 6
    */
#define PERIPHERAL_PHYS_BASE 0x20000000UL /* SPEC page 6 */
#elif defined(RPI2) || defined(RPI3)
    /* SEE: https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf
     *      ^ page 6
     */
    #define PERIPHERAL_PHYS_BASE 0x3F000000UL /* SPEC page 6 */
#elif defined(RPI4)
    /*
     *  SEE: https://github.com/raspberrypi/linux/blob/rpi-4.19.y/arch/arm/boot/dts/bcm2838.dtsi
     * ```ranges = <0x7e000000  0x0 0xfe000000  0x01800000>```
     */
    #define PERIPHERAL_PHYS_BASE 0xFE000000UL 
#else
    #error Unknown RPI model is being used.
#endif

#define GPIO_BASE (PERIPHERAL_PHYS_BASE + 0x00200000)
#define SPI0_BASE (PERIPHERAL_PHYS_BASE + 0x00204000)
/*
 * in SPEC, it says 0x3f20100 for UART base address but I believe it is a typo,
 * as both GPIO and SPI registers have additional '0' at the end
 */
#define UART0_BASE (PERIPHERAL_PHYS_BASE + 0x00201000)

/* ================== UART REGISTERS ================== */
/*
 * SEE SPEC Pages 177-178 for UART Address Map.
 */
/* UART Data register */
#define UART0_DR 0
/* UART Flag register */
#define UART0_FR 6
/* UART Integer Baud Rate divisor */
#define UART0_IBRD 9
/* UART Fractional baud rate divisor */
#define UART0_FBRD 10
/* UART Line control register */
#define UART0_LCRH 11
/* UART Control register */
#define UART0_CR 12
/* UART Interrupt Clear Register */
#define UART0_ICR 17
/* ==================================================== */

#endif /* PERIPHERALS */
