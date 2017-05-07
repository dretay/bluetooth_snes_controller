#pragma once

#include "my_ble_libs.h"

//advertising
extern ble_uuid_t m_adv_uuids[1];
extern pm_peer_id_t m_peer_id;                              /**< Device reference handle to the current bonded central. */


//services
extern ble_hids_t m_hids;                                   /**< Structure used to identify the HID service. */
extern ble_bas_t  m_bas;                                    /**< Structure used to identify the battery service. */
void keys_send(uint8_t key_pattern_len, uint16_t * p_key_pattern);


//joystick
#define JOYSTICK_REPORT_ID 0x03
#define JOYSTICK_INPUT_REPORT_KEYS_INDEX 0
#define JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN 4
#define JOYSTICK_INPUT_REP_REF_ID 3
#define SCAN_CODE_POS                    2                                           /**< This macro indicates the start position of the key scan code in a HID Report. As per the document titled 'Device Class Definition for Human Interface Devices (HID) V1.11, each report shall have one modifier byte followed by a reserved constant byte and then the key scan code. */

#define MAX_KEYS_IN_ONE_REPORT           (JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN - SCAN_CODE_POS) /**< Maximum number of key presses that can be sent in one Input Report. */
uint32_t send_key_scan_press_release(ble_hids_t*, uint16_t*, uint16_t, uint16_t, uint16_t*);

//timer
#define APP_TIMER_PRESCALER              0                                          /**< Value of the RTC1 PRESCALER register. */

//ble connection
#define CENTRAL_LINK_COUNT               0                                          /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT            1                                          /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
#define APP_FEATURE_NOT_SUPPORTED        BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */
extern uint16_t m_conn_handle;/**< Handle of the current connection. */
static bool m_is_wl_changed;                                      /**< Indicates if the whitelist has been changed since last time it has been updated in the Peer Manager. */
pm_peer_id_t   m_whitelist_peers[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];  /**< List of peers currently in the whitelist. */ 
uint32_t       m_whitelist_peer_cnt;                                 /**< Number of peers currently in the whitelist. */

