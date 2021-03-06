#include "my_ble_util.h"
#include "my_ble_advertising.h"
#include "my_ble_gap.h"
#include "my_ble_services.h"
#include "my_ble_connection.h"
#include "my_ble_buffer.h"
#include "my_ble_peer_manager.h"
#include "my_battery.h"
#include "my_keypad.h"
#include "my_timers.h"

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

/// <summary>
/// Maximum size of scheduler events.
/// </summary>
#define SCHED_MAX_EVENT_DATA_SIZE MAX(APP_TIMER_SCHED_EVT_SIZE, BLE_STACK_HANDLER_SCHED_EVT_SIZE)

/// <summary>
///  Maximum number of events in the scheduler queue.
/// </summary>
#define SCHED_QUEUE_SIZE 10

STATIC_ASSERT(sizeof(buffer_entry_t) % 4 == 0);


/// <summary>
/// callback for asserts in the softdevice
/// </summary>
/// <param name="line_num">line number of fault</param>
/// <param name="p_file_name">file name of fault</param>
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
	LOG("SoftDevice assert! %d, %s", line_num, p_file_name);
}


/// <summary>
/// initializes the task scheduler 
/// </summary>
static void scheduler_init(void) {
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/// <summary>
/// main application loop
/// </summary>
/// <returns>should never return</returns>
int main(void){
	uint32_t err_code;

	// Initialize.
	timers_init();
	ble_stack_init();
	scheduler_init();
	peer_manager_init(false);	
	gap_params_init();
	advertising_init();
	services_init();
	battery_sensor_init();
	conn_params_init();
	buffer_init();
	keypad_receiver_init();

	// Start execution.
	LOG("SNES Controller Start!\r\n");
	timers_start();
	advertising_start();

	// Enter main loop.
	for (;;){
		app_sched_execute();
		power_manage();
	}
}
