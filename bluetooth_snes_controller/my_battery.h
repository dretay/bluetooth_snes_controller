#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

static nrf_drv_adc_channel_t m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_2);

#define ADC_BUFFER_SIZE 6

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        2225
#define ADC_PRE_SCALING_COMPENSATION         3 
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
				((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 1023) * ADC_PRE_SCALING_COMPENSATION)																	 

/// <summary>
/// performs a battery measurement and updates the battery level characteristic in the battery service 
/// </summary>
void adc_sample(void);

/// /// <summary>
/// initializes the adc for voltage sensing
/// </summary>
void battery_sensor_init(void);

/// <summary>
/// schedule a battery level update - intended to be called from an irq (timer)
/// </summary>
void battery_level_update(void);