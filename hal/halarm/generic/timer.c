/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         BSD - See COPYING.ARM in the top level directory
 * FILE:            hal/halarm/generic/timer.c
 * PURPOSE:         Timer Routines
 * PROGRAMMERS:     ReactOS Portable Systems Group
 */

/* INCLUDES *******************************************************************/

#include <hal.h>
#define NDEBUG
#include <debug.h>

VOID
FASTCALL
KeUpdateSystemTime(
    IN PKTRAP_FRAME TrapFrame,
    IN ULONG Increment,
    IN KIRQL OldIrql
);

/* GLOBALS ********************************************************************/

ULONG HalpCurrentTimeIncrement, HalpNextTimeIncrement, HalpNextIntervalCount;
static ULONG HalpPerfCounterFrequency = 1000000;

/* PRIVATE FUNCTIONS **********************************************************/

VOID
HalpClockInterrupt(VOID)
{
    ASSERT(KeGetCurrentIrql() == CLOCK2_LEVEL);

    /*
     * Clear the timer interrupt.
     * The exact register depends on the ARM platform, but the pattern
     * is the same: acknowledge the timer and call KeUpdateSystemTime.
     */
    WRITE_REGISTER_ULONG(TIMER0_INT_CLEAR, 1);

    KeUpdateSystemTime(KeGetCurrentThread()->TrapFrame,
                       HalpCurrentTimeIncrement,
                       CLOCK2_LEVEL);
}

VOID
HalpStallInterrupt(VOID)
{
    WRITE_REGISTER_ULONG(TIMER0_INT_CLEAR, 1);
}

VOID
HalpInitializeClock(VOID)
{
    PKPCR Pcr = KeGetPcr();
    ULONG ClockInterval;
    SP804_CONTROL_REGISTER ControlRegister;

    Pcr->InterruptRoutine[CLOCK2_LEVEL] = HalpStallInterrupt;

    /*
     * Configure a 10ms periodic tick.
     * 10ms * 1MHz = 10000.
     */
    ClockInterval = 0x2710;

    ControlRegister.AsUlong = 0;
    ControlRegister.Wide = TRUE;
    ControlRegister.Periodic = TRUE;
    ControlRegister.Interrupt = TRUE;
    ControlRegister.Enabled = TRUE;

    WRITE_REGISTER_ULONG(TIMER0_LOAD, ClockInterval);
    WRITE_REGISTER_ULONG(TIMER0_CONTROL, ControlRegister.AsUlong);
}

/* PUBLIC FUNCTIONS ***********************************************************/

/*
 * @implemented
 */
VOID
NTAPI
HalCalibratePerformanceCounter(IN volatile PLONG Count,
                               IN ULONGLONG NewCount)
{
    UNREFERENCED_PARAMETER(NewCount);

    if (Count)
    {
        *Count = 0;
    }
}

/*
 * @implemented
 */
ULONG
NTAPI
HalSetTimeIncrement(IN ULONG Increment)
{
    HalpCurrentTimeIncrement = Increment;
    HalpNextTimeIncrement = Increment;
    KeSetTimeIncrement(Increment, Increment / 10);

    return Increment;
}

/*
 * @implemented
 */
VOID
NTAPI
KeStallExecutionProcessor(IN ULONG Microseconds)
{
    SP804_CONTROL_REGISTER ControlRegister;

    WRITE_REGISTER_ULONG(TIMER1_LOAD, Microseconds);

    ControlRegister.AsUlong = 0;
    ControlRegister.OneShot = TRUE;
    ControlRegister.Wide = TRUE;
    ControlRegister.Periodic = TRUE;
    ControlRegister.Enabled = TRUE;
    WRITE_REGISTER_ULONG(TIMER1_CONTROL, ControlRegister.AsUlong);

    while (READ_REGISTER_ULONG(TIMER1_VALUE))
    {
        /* spin until the timer expires */
    }
}

/*
 * @implemented
 */
LARGE_INTEGER
NTAPI
KeQueryPerformanceCounter(IN PLARGE_INTEGER PerformanceFreq)
{
    LARGE_INTEGER Value;

    if (PerformanceFreq)
    {
        *PerformanceFreq = RtlConvertLongToLargeInteger(HalpPerfCounterFrequency);
    }

    /*
     * Use the hardware counter register for the actual raw tick value.
     * The concrete register name differs by board, but the pattern is:
     *   Value.QuadPart = READ_REGISTER_ULONG(TIMER_COUNTER);
     */
    Value.QuadPart = READ_REGISTER_ULONG(TIMER0_VALUE);

    return Value;
}

/* EOF */
