#include "my_ble_util.h"
#include "my_ble_advertising.h"
#include "my_ble_gap.h"
#include "my_ble_services.h"
#include "my_ble_connection.h"
#include "my_ble_buffer.h"
#include "my_ble_peer_manager.h"
#include "my_battery.h"
#include "my_keypad.h"

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_TIMER_OP_QUEUE_SIZE          6                                          /**< Size of timer operation queues. */
#define BATTERY_LEVEL_MEAS_INTERVAL      APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER) /**< Battery level measurement interval (ticks). */
#define DEAD_BEEF                        0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define SCHED_MAX_EVENT_DATA_SIZE        MAX(APP_TIMER_SCHED_EVT_SIZE, \
                                             BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                 10                                          /**< Maximum number of events in the scheduler queue. */

STATIC_ASSERT(sizeof(buffer_entry_t) % 4 == 0);


APP_TIMER_DEF(m_battery_timer_id);                          /**< Battery timer. */


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
	app_error_handler(DEAD_BEEF, line_num, p_file_name);
}



/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context) {
	UNUSED_PARAMETER(p_context);
	battery_level_update();
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void) {
	uint32_t err_code;

	// Initialize timer module, making it use the scheduler.
	APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);

	// Create battery timer.
	err_code = app_timer_create(&m_battery_timer_id,APP_TIMER_MODE_REPEATED,battery_level_meas_timeout_handler);
	APP_ERROR_CHECK(err_code);
}




/**@brief Function for starting timers.
 */
static void timers_start(void) {
	uint32_t err_code;

	err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}



/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void) {
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


int main(void)
{
	bool     erase_bonds;
	uint32_t err_code;

	// Initialize.
	timers_init();
	ble_stack_init();
	scheduler_init();
	peer_manager_init(true);	
	gap_params_init();
	advertising_init();
	services_init();
	battery_sensor_init();
	conn_params_init();
	buffer_init();
	keypad_receiver_init();

	// Start execution.
	LOG("HID Keyboard Start!\r\n");
	timers_start();
	advertising_start();

	// Enter main loop.
	for (;;){
		app_sched_execute();
		//power_manage();
	}
}
