#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

/// <summary>
/// name of device. Will be included in the advertising data
/// </summary>
#define DEVICE_NAME "SNES_GAMEPAD"

/// <summary>
/// minimum connection interval (7.5 ms)
/// </summary>
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(7.5, UNIT_1_25_MS)

/// <summary>
/// max connection interval (30 ms).
/// </summary>
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(30, UNIT_1_25_MS)

/// <summary>
/// slave latency
/// </summary>
#define SLAVE_LATENCY 6

/// <summary>
/// connection supervisory timeout (430 ms)
/// </summary>
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(430, UNIT_10_MS)

/// <summary>
/// initializes gap - sets all "Generic Access Profile" parameters of the device including the device name, appearance, and the preferred connection parameters.
/// </summary>
void gap_params_init(void);