#pragma once

//nordic stuff
#include "my_ble_globals.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf_drv_uart.h"

//my stuff
#include "my_ble_util.h"
#include "my_timers.h"

//keypad stuff
typedef struct {
	uint8_t idx;
	char* s;
} keypad_button_t;
#define NUM_KEYPAD_BUTTONS 12
uint16_t keypad_state;

//uart stuff
extern nrf_drv_uart_t uart_driver_instance;
static uint8_t rx_buffer[2];
	
/// <summary>
/// initializes attached keypad 
/// </summary>
void keypad_receiver_init(void);

/// <summary>
/// handler for receiving a uart msg
/// </summary>
/// <param name="p_event">uart event structure</param>
/// <param name="p_context">initialization context</param>
static void uart_event_handler(nrf_drv_uart_event_t * p_event, void* p_context);
