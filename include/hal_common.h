#pragma once

/* Declared, not #included: keeps this base header free of a dependency on
 * the fault module, which sits above the drivers. See fault.h. */
void fault_blink(void);

typedef enum
{
    HAL_OK = 0,
    HAL_ERROR,
    HAL_BUSY,
    HAL_TIMEOUT
} HAL_Status;

// propagates failure
#define HAL_TRY(expr)                \
    do                               \
    {                                \
        HAL_Status _status = (expr); \
        if (_status != HAL_OK)       \
        {                            \
            return _status;          \
        }                            \
    } while (0)

// halts
#define HAL_CHECK(expr)              \
    do                               \
    {                                \
        HAL_Status _status = (expr); \
        if (_status != HAL_OK)       \
        {                            \
            fault_blink();           \
        }                            \
    } while (0)
