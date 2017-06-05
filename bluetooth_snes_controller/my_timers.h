#pragma once

//nordic stuff
#include "my_ble_globals.h"

//my stuff
#include "my_battery.h"

/// <summary>
/// battery measurement interval (every 1 min)
/// </summary>
#define BATTERY_LEVEL_MEAS_INTERVAL APP_TIMER_TICKS(60000, APP_TIMER_PRESCALER)
APP_TIMER_DEF(m_battery_timer_id);

/// <summary>
/// inactivity timeout interval (should be 5 min)
/// </summary>
#define INACTIVITY_SLEEP_DELAY APP_TIMER_TICKS(300000, APP_TIMER_PRESCALER)
APP_TIMER_DEF(m_shutdown_timer_id);

/// <summary>
/// size of timer operation queues
/// </summary>
#define APP_TIMER_OP_QUEUE_SIZE 6

/// <summary>
/// handles battery timeout irq
/// </summary>
/// <param name="p_context">event context</param>
void battery_level_meas_timeout_handler(void * p_context);

/// <summary>
/// handles shutdown timeout irq
/// </summary>
/// <param name="p_context">event context</param>
void shutdown_timeout_handler(void * p_context);

/// <summary>
/// necessary setup for timers to initialize
/// </summary>
void timers_init(void);

/// <summary>
/// start the timers
/// </summary>
void timers_start(void);

/// <summary>
/// reset the expiration for the shutdown timer
/// </summary>
void reset_shutdown_timer(void);
