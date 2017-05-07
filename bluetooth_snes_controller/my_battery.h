#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

static nrf_drv_adc_channel_t m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_2);

#define ADC_BUFFER_SIZE 6

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        2225
#define ADC_PRE_SCALING_COMPENSATION         3 
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
				((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 1023) * ADC_PRE_SCALING_COMPENSATION)																	 

void adc_sample(void);
void battery_sensor_init(void);
void battery_level_update(void);