#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"
#include "my_ble_buffer.h"

/// <summary>
/// vendor id source
/// </summary>
#define PNP_ID_VENDOR_ID_SOURCE 0x02

/// <summary>
/// vendor id
/// </summary>
#define PNP_ID_VENDOR_ID 0x1915

/// <summary>
/// product id
/// </summary>
#define PNP_ID_PRODUCT_ID 0xEEEE

/// <summary>
/// product version
/// </summary>
#define PNP_ID_PRODUCT_VERSION 0x0001

/// <summary>
/// manufacturer name
/// </summary>
#define MANUFACTURER_NAME "NordicSemiconductor"

/// <summary>
/// Version number of base USB HID Specification implemented
/// </summary>
#define BASE_USB_HID_SPEC_VERSION 0x0101

/// <summary>
/// protocol mode
/// </summary>
static bool m_in_boot_mode = false;

/// <summary>
/// handles HID events
/// </summary>
/// <param name="p_hids">HID service structure</param>
/// <param name="p_evt">event returned from HID service</param>
static void on_hids_evt(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);

/// <summary>
/// handles service errors
/// </summary>
/// <param name="nrf_error">code containing information about what went wrong</param>
static void service_error_handler(uint32_t nrf_error);

/// <summary>
/// initializes services that will be used by the application
/// </summary>
void services_init(void);

/// <summary>
/// initializes the Device Information Service
/// </summary>
static void dis_init(void);

/// <summary>
/// initializes the battery service
/// </summary>
static void bas_init(void);

/// <summary>
/// initializes the hid service
/// </summary>
static void gamepad_hids_init(void);

/// <summary>
/// transmit a key press notification.
/// </summary>
/// <param name="p_hids">service for which key press notifications are requested</param>
/// <param name="p_key_pattern">pointer to key pattern</param>
/// <param name="pattern_len">length of key pattern</param>
/// <param name="pattern_offset">offset applied to Key Pattern for transmission</param>
/// <param name="p_actual_len">actual length of Key Pattern transmitted, making buffering of rest possible if needed</param>
/// <returns>NRF_SUCCESS on success, BLE_ERROR_NO_TX_PACKETS in case transmission could not be completed </returns>
uint32_t send_key_scan_press_release(ble_hids_t * p_hids, uint16_t* p_key_pattern, uint16_t pattern_len, uint16_t pattern_offset, uint16_t* p_actual_len);

/// <summary>
/// send key presses to the peer
/// </summary>
/// <param name="key_pattern_len">pattern length</param>
/// <param name="p_key_pattern">pattern to send</param>
void keys_send(uint8_t key_pattern_len, uint16_t * p_key_pattern);

/// <summary>
/// handle the HID Report Characteristic Write event
/// </summary>
/// <param name="p_evt">HID service event</param>
static void on_hid_rep_char_write(ble_hids_evt_t * p_evt);