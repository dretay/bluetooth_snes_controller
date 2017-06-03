#include "my_timers.h"


//called each time battery timeout expires
void battery_level_meas_timeout_handler(void * p_context) {
	UNUSED_PARAMETER(p_context);
	battery_level_update();
}

void shutdown_timeout_handler(void * p_context) {
	uint32_t err_code;
	UNUSED_PARAMETER(p_context);
	LOG("SHUTTING DOWN!");
	
	//err_code = app_timer_stop(m_shutdown_timer_id);
	//APP_ERROR_CHECK(err_code);
	sd_power_system_off();
}

//timer init
void timers_init(void) {
	uint32_t err_code;

	// Initialize timer module, making it use the scheduler.
	APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);

	// Create battery timer.
	err_code = app_timer_create(&m_battery_timer_id, APP_TIMER_MODE_REPEATED, battery_level_meas_timeout_handler);
	APP_ERROR_CHECK(err_code);

	// Create shutdown timer.
	err_code = app_timer_create(&m_shutdown_timer_id, APP_TIMER_MODE_REPEATED, shutdown_timeout_handler);
	APP_ERROR_CHECK(err_code);
}




/**@brief Function for starting timers.
 */
void timers_start(void) {
	uint32_t err_code;

	err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_start(m_shutdown_timer_id, INACTIVITY_SLEEP_DELAY, NULL);
	APP_ERROR_CHECK(err_code);
}

void reset_shutdown_timer(void){
	uint32_t err_code;
	err_code = app_timer_stop(m_shutdown_timer_id);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_shutdown_timer_id, INACTIVITY_SLEEP_DELAY, NULL);
	APP_ERROR_CHECK(err_code);
}