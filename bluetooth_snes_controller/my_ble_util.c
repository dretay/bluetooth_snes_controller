#include "my_ble_util.h"

void power_manage(void){
	uint32_t err_code = sd_app_evt_wait();
	APP_ERROR_CHECK(err_code);
}

void sleep_mode_enter(void){
	// Go to system-off mode (this function will not return; wakeup will cause a reset).
	uint32_t err_code = sd_power_system_off();
	APP_ERROR_CHECK(err_code);
}