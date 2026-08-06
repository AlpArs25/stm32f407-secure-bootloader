#include "led.h"
#include "fault.h"

void fault_blink(void)
{
    led_setup();
    while (1)
    {
        led_toggle(RED);
        for (volatile int d = 0; d < 300000; d++)
        {
        }
        led_toggle(RED);
        for (volatile int d = 0; d < 300000; d++)
        {
        }
    }
}
