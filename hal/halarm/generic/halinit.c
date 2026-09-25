/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         BSD - See COPYING.ARM in the top level directory
 * FILE:            hal/halarm/generic/halinit.c
 * PURPOSE:         HAL Entrypoint and Initialization
 * PROGRAMMERS:     ReactOS Portable Systems Group
 */

/* INCLUDES *******************************************************************/

#include <hal.h>
#define NDEBUG
#include <debug.h>

/* GLOBALS ********************************************************************/

/* PRIVATE FUNCTIONS **********************************************************/

static
CODE_SEG("INIT")
VOID
HalpGetParameters(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    if (LoaderBlock && LoaderBlock->LoadOptions)
    {
        PCSTR CommandLine = LoaderBlock->LoadOptions;

        if (strstr(CommandLine, "BREAK"))
            DbgBreakPoint();
    }
}

static
VOID
HalpInitPhase0(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    UNREFERENCED_PARAMETER(LoaderBlock);

    /*
     * ARM HAL bootstrap sequence.
     * This is the minimum initialization required before the kernel
     * can safely start using the HAL.
     */
    HalpInitializeClock();
    HalpInitializeInterrupts();

    /*
     * Ensure the interrupt framework is in a sane state before we
     * start using device timers and profile interrupts.
     */
    KfRaiseIrql(KeGetCurrentIrql());
}

static
VOID
HalpInitPhase1(VOID)
{
    /*
     * Board-level and device-level initialization.
     * The exact devices are board-specific and are implemented in
     * hal/halarm/<board>/halinit_up.c.
     */
    HalpInitializeInterrupts();
    HalpInitializeDma();
}

/* FUNCTIONS ******************************************************************/

/*
 * @implemented
 */
CODE_SEG("INIT")
BOOLEAN
NTAPI
HalInitSystem(
    _In_ ULONG BootPhase,
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    PKPRCB Prcb = KeGetCurrentPrcb();

    if (BootPhase == 0)
    {
        HalpGetParameters(LoaderBlock);

#if DBG
        if (!(Prcb->BuildType & PRCB_BUILD_DEBUG))
        {
            KeBugCheckEx(MISMATCHED_HAL, 2, Prcb->BuildType, PRCB_BUILD_DEBUG, 0);
        }
#else
        if (Prcb->BuildType & PRCB_BUILD_DEBUG)
        {
            KeBugCheckEx(MISMATCHED_HAL, 2, Prcb->BuildType, 0, 0);
        }
#endif

#ifdef CONFIG_SMP
        if (Prcb->BuildType & PRCB_BUILD_UNIPROCESSOR)
        {
            KeBugCheckEx(MISMATCHED_HAL, 2, Prcb->BuildType, 0, 0);
        }
#endif

        if (Prcb->MajorVersion != PRCB_MAJOR_VERSION)
        {
            KeBugCheckEx(MISMATCHED_HAL, 1, Prcb->MajorVersion, PRCB_MAJOR_VERSION, 0);
        }

        HalpInitializeInterrupts();
        KfRaiseIrql(KeGetCurrentIrql());

        /*
         * Fill out the dispatch tables.
         * These are required for kernel/driver-call HAL entry points.
         */
        HalQuerySystemInformation = HaliQuerySystemInformation;
        HalSetSystemInformation = HaliSetSystemInformation;
        HalInitPnpDriver = HaliInitPnpDriver;
        HalGetDmaAdapter = HalpGetDmaAdapter;
        HalGetInterruptTranslator = HalpGetInterruptTranslator;
        HalResetDisplay = HalpDisplayReset;
        HalHaltSystem = HaliHaltSystem;

        HalpInitializeClock();

        HalpCurrentTimeIncrement = 100000;
        HalpNextTimeIncrement = 100000;
        HalpNextIntervalCount = 0;
        KeSetTimeIncrement(100000, 10000);

        HalStopProfileInterrupt(ProfileTime);

        HalpInitPhase0(LoaderBlock);
    }
    else if (BootPhase == 1)
    {
        HalpInitPhase1();
    }

    return TRUE;
}

/* EOF */
