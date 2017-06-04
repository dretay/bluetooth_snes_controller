#pragma once

#include "my_ble_libs.h"
#include "my_ble_util.h"
#include "my_ble_globals.h"

#define APP_ADV_FAST_INTERVAL            0x0028                                     /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL            0x0C80                                     /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */
#define APP_ADV_FAST_TIMEOUT             30                                         /**< The duration of the fast advertising period (in seconds). */
#define APP_ADV_SLOW_TIMEOUT             180                                        /**< The duration of the slow advertising period (in seconds). */

/// <summary>
/// function for initializing the Advertising functionality
/// </summary>
void advertising_init(void);

/// <summary>
/// function for starting advertising.
/// </summary>
void advertising_start(void);

/// <summary>
/// fetch the list of peer manager peer IDs
/// </summary>
/// <param name="p_peers">The buffer where to store the list of peer IDs</param>
/// <param name="p_size">The size of the p_peers buffer</param>
static void peer_list_get(pm_peer_id_t* p_peers, uint32_t* p_size);

/// <summary>
/// function for handling advertising errors
/// </summary>
/// <param name="nrf_error">information about what went wrong.</param>
static void ble_advertising_error_handler(uint32_t nrf_error);

/// <summary>
/// handles advertising events - called for advertising events which are passed to the application
/// </summary>
/// <param name="ble_adv_evt">advertising event</param>
static void on_adv_evt(ble_adv_evt_t ble_adv_evt);