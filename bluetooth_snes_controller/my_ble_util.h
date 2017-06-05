#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_WRITE(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_TOGGLR(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) (((x) & (y)) == (y))

/// <summary>
/// put system to sleep mode
/// </summary>
void sleep_mode_enter(void);

/// <summary>
/// function for power manager
/// </summary>
void power_manage(void);