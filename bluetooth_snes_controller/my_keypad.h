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
	
void buttons_init(void);
void keypad_receiver_init(void);
