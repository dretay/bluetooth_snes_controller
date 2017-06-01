#include "my_battery.h"

static nrf_adc_value_t       adc_buffer[ADC_BUFFER_SIZE];
static uint8_t adc_event_counter = 0;

static void adc_event_handler(nrf_drv_adc_evt_t const * p_event) {
	uint32_t err_code;
	uint16_t adc_sum_value = 0;
	uint16_t adc_average_value;
	uint16_t adc_result_millivolts;
	uint8_t  adc_result_percent;

	sd_clock_hfclk_release();			//Release the external crystal
	
	adc_event_counter++;
//	LOG("    ADC event counter: %d\r\n", adc_event_counter);
	if (p_event->type == NRF_DRV_ADC_EVT_DONE) {
		uint32_t i;
		for (i = 0; i < p_event->data.done.size; i++) {
//			LOG("Sample value %d: %d\r\n", i + 1, p_event->data.done.p_buffer[i]);
			adc_sum_value += p_event->data.done.p_buffer[i];                           //Sum all values in ADC buffer
		}
		adc_average_value = adc_sum_value / p_event->data.done.size;                   //Calculate average value from all samples in the ADC buffer
//		LOG("Average ADC value: %d\r\n", adc_average_value);
				
		adc_result_millivolts = ADC_RESULT_IN_MILLI_VOLTS(adc_average_value);          //Transform the average ADC value into millivolts value
//		LOG("ADC result in millivolts: %d\r\n", adc_result_millivolts);
				
		adc_result_percent = battery_level_in_percent(adc_result_millivolts);          //Transform the millivolts value into battery level percent.
		LOG("Battery level: %d\r\n", adc_result_percent);
				
        //Send the battery level over BLE
		err_code = ble_bas_battery_level_update(&m_bas, adc_result_percent);           //Send the battery level over BLE
		if ((err_code != NRF_SUCCESS) &&
		    (err_code != NRF_ERROR_INVALID_STATE) &&                                   
		    (err_code != BLE_ERROR_NO_TX_PACKETS) &&                                   
		    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
			APP_ERROR_HANDLER(err_code);                                               //Assert on error
		}
	}
}

/**@brief Function for initializing the battery sensor simulator.
 */
void battery_sensor_init(void)
{
	ret_code_t ret_code;
	static nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;	
	
	ret_code = nrf_drv_adc_init(&config, adc_event_handler);
	APP_ERROR_CHECK(ret_code);
	
	nrf_drv_adc_channel_enable(&m_channel_config);
}

/**@brief Function for performing a battery measurement, and update the Battery Level characteristic in the Battery Service.
 */
void adc_sample(void) {
	ret_code_t ret_code;
	uint32_t p_is_running = 0;
	
	ret_code = nrf_drv_adc_buffer_convert(adc_buffer, ADC_BUFFER_SIZE);       // Allocate buffer for ADC
	APP_ERROR_CHECK(ret_code);
	
    //Request the external high frequency crystal for best ADC accuracy. For lowest current consumption, don't request the crystal.
	sd_clock_hfclk_request();
	while (!p_is_running) {          //wait for the hfclk to be available
		sd_clock_hfclk_is_running((&p_is_running));
	}  
	
	for (uint32_t i = 0; i < ADC_BUFFER_SIZE; i++) {
		while ((NRF_ADC->BUSY & ADC_BUSY_BUSY_Msk) == ADC_BUSY_BUSY_Busy) {}   //Wait until the ADC is finished sampling
//		LOG("Start sampling ... \r\n");
		nrf_drv_adc_sample();        // Trigger ADC conversion
	}					
}

void battery_level_update(void)
{
//	LOG("\r\n    Triggering battery level update...\r\n");
	app_sched_event_put(0, 0, (app_sched_event_handler_t)adc_sample);
}