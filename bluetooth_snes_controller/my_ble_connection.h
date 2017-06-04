#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

/// <summary>
/// time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds) 
/// </summary>
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)

/// <summary>
/// time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds)
/// </summary>
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)

/// <summary>
/// number of attempts before giving up the connection parameter negotiation
/// </summary>
#define MAX_CONN_PARAMS_UPDATE_COUNT 3 

/// <summary>
/// handling the Application's BLE Stack events
/// </summary>
/// <param name="p_ble_evt">stack event</param>
static void on_ble_evt(ble_evt_t * p_ble_evt); 

/// <summary>
/// handles a Connection Parameters error
/// </summary>
/// <param name="nrf_error">what went wrong</param>
static void conn_params_error_handler(uint32_t nrf_error);

/// <summary>
/// dispatches BLE stack events to all modules with a BLE stack event handler
/// called from the scheduler in the main loop after a BLE stack event has been received.
/// </summary>
/// <param name="p_ble_evt">stack event</param>
static void ble_evt_dispatch(ble_evt_t * p_ble_evt);

/// <summary>
/// dispatches system events to interested modules.  called from the System event interrupt handler after a system event has been received
/// </summary>
/// <param name="sys_evt">stack event</param>
static void sys_evt_dispatch(uint32_t sys_evt);

/// <summary>
/// initializes the Connection Parameters module
/// </summary>
void conn_params_init(void);

/// <summary>
/// inits the BLE stack (softdevice and ble irq)
/// </summary>
void ble_stack_init(void);