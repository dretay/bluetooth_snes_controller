#pragma once

//libc
#include <stdint.h>
#include <string.h>

//nordic system libs
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpio.h"
#include "nrf_drv_adc.h"

#include "app_error.h"
#include "app_scheduler.h"
#include "app_util.h"
#include "app_timer_appsh.h"
#include "app_button.h"

#include "fds.h"
#include "fstorage.h"


//nordic bluetooth libs
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advertising.h"
#include "ble_advdata.h"
#include "ble_hids.h"
#include "ble_bas.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"

#include "softdevice_handler_appsh.h"
#include "peer_manager.h"

//my libs
#include "log.h"
#include "my_ble_util.h"
#include "my_ble_buffer.h"
