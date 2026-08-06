#pragma once
#include "fault.h"

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
