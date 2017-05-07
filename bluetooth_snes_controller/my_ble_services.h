#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"
#include "my_ble_buffer.h"

#define PNP_ID_VENDOR_ID_SOURCE          0x02                                       /**< Vendor ID Source. */
#define PNP_ID_VENDOR_ID                 0x1915                                     /**< Vendor ID. */
#define PNP_ID_PRODUCT_ID                0xEEEE                                     /**< Product ID. */
#define PNP_ID_PRODUCT_VERSION           0x0001                                     /**< Product Version. */
#define MANUFACTURER_NAME                "NordicSemiconductor"                      /**< Manufacturer. Will be passed to Device Information Service. */



#define BASE_USB_HID_SPEC_VERSION        0x0101                                      /**< Version number of base USB HID Specification implemented by this application. */




static bool       m_in_boot_mode = false;                   /**< Current protocol mode. */

static void on_hids_evt(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);
static void service_error_handler(uint32_t nrf_error);
void services_init(void);
