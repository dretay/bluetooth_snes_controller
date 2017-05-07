#include "my_keypad.h"

nrf_drv_uart_t uart_driver_instance = NRF_DRV_UART_INSTANCE(UART0_INSTANCE_INDEX);
keypad_button_t keypad_buttons[] = {
	{ 0,  "1" },
	{ 1,  "2" },
	{ 2,  "3" },
	{ 3,  "4" },
	{ 4,  "5" },
	{ 5,  "6" },
	{ 6,  "7" },
	{ 7,  "8" },
	{ 8,  "9" },
	{ 9,  "10" },
	{ 10, "11" },
	{ 11, "12" }
};

static void uart_event_handler(nrf_drv_uart_event_t * p_event, void* p_context) {	
	uint32_t err_code;
	uint16_t combined_state = 0;
	uint8_t i = 0;

	switch (p_event->type) {
	case NRF_DRV_UART_EVT_RX_DONE:
		combined_state = rx_buffer[0];
		combined_state = combined_state << 8;
		combined_state |= rx_buffer[1];
		
		
		if (combined_state != keypad_state) {
			keypad_state = combined_state;
			for (i = 0; i < NUM_KEYPAD_BUTTONS; i++) {
				if (BIT_CHECK(keypad_state, i)) {
					LOG("%s", keypad_buttons[i].s);		
				}
				else {
					LOG("-");
				}
				if (i + 1 == NUM_KEYPAD_BUTTONS) {
					LOG("\n");
				}
			}
			keys_send(sizeof(keypad_state), &keypad_state);
		}
		nrf_drv_uart_rx(&uart_driver_instance, rx_buffer, 2);
		break;

	case NRF_DRV_UART_EVT_ERROR:	
		LOG("NRF_DRV_UART_EVT_ERROR");
	
		break;

	case NRF_DRV_UART_EVT_TX_DONE:	    
		LOG("NRF_DRV_UART_EVT_TX_DONE");

		break;

	default:
		break;
	}
}


void keypad_receiver_init(void) {
	uint32_t err_code;

	nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
	config.baudrate = NRF_UART_BAUDRATE_4800;
	config.pselrxd = 9;
	err_code = nrf_drv_uart_init(&uart_driver_instance, &config, uart_event_handler);
	nrf_drv_uart_rx_enable(&uart_driver_instance);
	nrf_drv_uart_rx(&uart_driver_instance, rx_buffer, 2);

}
bool i2c_tx(void *i2c_instance, uint8_t i2c_address, uint8_t *message, uint8_t size) {

}