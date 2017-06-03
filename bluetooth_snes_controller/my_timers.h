#pragma once

//nordic stuff
#include "my_ble_globals.h"

//my stuff
#include "my_battery.h"

#define BATTERY_LEVEL_MEAS_INTERVAL      APP_TIMER_TICKS(60000, APP_TIMER_PRESCALER) /**< Battery level measurement interval (every 1m). */
#define INACTIVITY_SLEEP_DELAY			 APP_TIMER_TICKS(300000, APP_TIMER_PRESCALER) /** Should be 5m */

APP_TIMER_DEF(m_battery_timer_id);                          /**< Battery timer. */
APP_TIMER_DEF(m_shutdown_timer_id);
#define APP_TIMER_OP_QUEUE_SIZE          6                                          /**< Size of timer operation queues. */

void battery_level_meas_timeout_handler(void * p_context);
void shutdown_timeout_handler(void * p_context);
void timers_init(void);
void timers_start(void);

void reset_shutdown_timer(void);
