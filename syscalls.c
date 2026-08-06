#include <stdint.h>
#include <errno.h>
#include "hal_common.h"
#include "usart.h"
#include "stm32f407xx.h"

int _write(int fd, char *ptr, int len);
void *_sbrk(ptrdiff_t incr);

// USART2 default
int _write(int fd, char *ptr, int len)
{
    (void)fd; // suppress warning
    // newlib expects a byte count as error so we have to use byte version
    for (int i = 0; i < len; i++)
    {
        HAL_Status status = usart_write_byte(USART2, ptr[i]);
        if (status != HAL_OK)
        {
            return i;
        }
    }
    return len;
}

extern uint8_t end;
extern uint8_t __heap_limit;

void *_sbrk(ptrdiff_t incr)
{
    static uint8_t *heap = &end;
    uint8_t *heap_limit = &__heap_limit;

    if ((heap + incr > heap_limit) || (heap + incr < &end))
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    void *old_heap = heap;
    heap += incr;
    return old_heap;
}
