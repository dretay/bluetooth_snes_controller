#pragma once

#include "my_ble_libs.h"
#include "my_ble_util.h"
#include "my_ble_globals.h"

#define APP_ADV_FAST_INTERVAL            0x0028                                     /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL            0x0C80                                     /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */
#define APP_ADV_FAST_TIMEOUT             30                                         /**< The duration of the fast advertising period (in seconds). */
#define APP_ADV_SLOW_TIMEOUT             180                                        /**< The duration of the slow advertising period (in seconds). */

void advertising_init(void);
void advertising_start(void);
static void peer_list_get(pm_peer_id_t*, uint32_t*);