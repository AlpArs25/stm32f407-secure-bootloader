#include <stdint.h>
#include "hal_common.h"
#include "usart.h"
#include "stm32f407xx.h"

int _write(int fd, char *ptr, int len);

// USART2 default
int _write(int fd, char *ptr, int len)
{
    (void)fd; // suppress warning
    // newlib expects a byte count as error so we have to use byte version
    for (int i = 0; i < len; i++)
    {
        HAL_Status status = usart_write_byte(USART2, &ptr[i]);
        if (status != HAL_OK)
        {
            return i;
        }
    }
    return len;
}