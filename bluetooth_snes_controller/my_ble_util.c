#include "my_ble_util.h"

/**@brief Function for the Power manager.
 */
void power_manage(void){
	uint32_t err_code = sd_app_evt_wait();

	APP_ERROR_CHECK(err_code);
}
/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void){
//	uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
//
//	APP_ERROR_CHECK(err_code);
//
//	    // Prepare wakeup buttons.
//	err_code = bsp_btn_ble_sleep_mode_prepare();
//	APP_ERROR_CHECK(err_code);

	    // Go to system-off mode (this function will not return; wakeup will cause a reset).
	uint32_t err_code = sd_power_system_off();
	APP_ERROR_CHECK(err_code);
}