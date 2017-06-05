#pragma once

#include "my_ble_libs.h"
#include "my_ble_advertising.h"
#include "my_ble_globals.h"

/// <summary>
/// perform bonding
/// </summary>
#define SEC_PARAM_BOND 1

/// <summary>
/// Man In The Middle protection enabled?
/// </summary>
#define SEC_PARAM_MITM 0

/// <summary>
/// LE Secure Connections enabled?
/// </summary>
#define SEC_PARAM_LESC 0

/// <summary>
/// keypress notifications
/// </summary>
#define SEC_PARAM_KEYPRESS 0

/// <summary>
/// io capabilities (keyboard, screen...)
/// </summary>
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_NONE

/// <summary>
/// out of band data enabled?
/// </summary>
#define SEC_PARAM_OOB 0

/// <summary>
/// min encryption key size
/// </summary>
#define SEC_PARAM_MIN_KEY_SIZE 7

/// <summary>
/// max encryption key size
/// </summary>
#define SEC_PARAM_MAX_KEY_SIZE 16

/// <summary>
/// peer manager init
/// </summary>
/// <param name="erase_bonds">whether bonding information should be cleared from persistent storage during initialization of the Peer Manage</param>
void peer_manager_init(bool erase_bonds);

/// <summary>
/// handes Peer Manager events
/// </summary>
/// <param name="p_evt">peer manager event</param>
static void pm_evt_handler(pm_evt_t const * p_evt);