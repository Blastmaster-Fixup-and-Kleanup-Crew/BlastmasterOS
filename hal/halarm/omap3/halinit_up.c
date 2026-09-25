/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         BSD - See COPYING.ARM in the top level directory
 * FILE:            hal/halarm/omap3/halinit_up.c
 * PURPOSE:         OMAP3 Board-Specific HAL Initialization
 */

#include <hal.h>
#define NDEBUG
#include <debug.h>

VOID
HalpInitPhase0(IN PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    UNREFERENCED_PARAMETER(LoaderBlock);

    HalpInitializeClock();
    HalpInitializeInterrupts();

    /*
     * OMAP3 board init should configure:
     * - timer device
     * - UART/console
     * - interrupt controller
     */
}

VOID
HalpInitPhase1(VOID)
{
    HalpInitializeDma();
    HalpInitializeInterrupts();
}

/* EOF */
