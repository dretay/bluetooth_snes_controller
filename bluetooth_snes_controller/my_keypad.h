#pragma once

//nordic stuff
#include "my_ble_globals.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf_drv_uart.h"
//my stuff
#include "my_ble_util.h"

//keypad stuff
typedef struct {
	uint8_t idx;
	char* s;
} keypad_button_t;
#define NUM_KEYPAD_BUTTONS 12

//i2c stuff
#define KEYPAD_SCL_PIN 8
#define KEYPAD_SDA_PIN 9
#define KEYPAD_IRQ_PIN 10
#define KEYPAD_RST_PIN 11
#define TWI_INSTANCE_ID 0

//uart stuff
extern nrf_drv_uart_t uart_driver_instance;
static uint8_t rx_buffer[2];


//keypad config
#define MAX_KEYPAD_BUTTONS 12

extern uint16_t KEYPAD_STATE;

#define APP_TICKS_PER_100MS APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)
uint16_t keypad_state;
#define KEYPAD_MEAS_INTERVAL      APP_TIMER_TICKS(100, APP_TIMER_PRESCALER)
APP_TIMER_DEF(m_keypad_timer_id);
	
void buttons_init(void);
void keypad_receiver_init(void);
