#include "my_ble_services.h"

ble_hids_t m_hids;                                   /**< Structure used to identify the HID service. */
ble_bas_t  m_bas;                                    /**< Structure used to identify the battery service. */
bool         m_caps_on = false;                      /**< Variable to indicate if Caps Lock is turned on. */


/**@brief Function for initializing Device Information Service.
 */
static void dis_init(void){
	uint32_t         err_code;
	ble_dis_init_t   dis_init_obj;
	ble_dis_pnp_id_t pnp_id;

	pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE;
	pnp_id.vendor_id        = PNP_ID_VENDOR_ID;
	pnp_id.product_id       = PNP_ID_PRODUCT_ID;
	pnp_id.product_version  = PNP_ID_PRODUCT_VERSION;

	memset(&dis_init_obj, 0, sizeof(dis_init_obj));

	ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME);
	dis_init_obj.p_pnp_id = &pnp_id;

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&dis_init_obj.dis_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init_obj.dis_attr_md.write_perm);

	err_code = ble_dis_init(&dis_init_obj);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing Battery Service.
 */
static void bas_init(void){
	uint32_t       err_code;
	ble_bas_init_t bas_init_obj;

	memset(&bas_init_obj, 0, sizeof(bas_init_obj));

	bas_init_obj.evt_handler          = NULL;
	bas_init_obj.support_notification = true;
	bas_init_obj.p_report_ref         = NULL;
	bas_init_obj.initial_batt_level   = 100;

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_char_attr_md.cccd_write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_char_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init_obj.battery_level_char_attr_md.write_perm);

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_report_read_perm);

	err_code = ble_bas_init(&m_bas, &bas_init_obj);
	APP_ERROR_CHECK(err_code);
}

//initialize the hid service
static void gamepad_hids_init(void) {	
	
	uint32_t                   err_code;
	ble_hids_init_t            hids_init_obj;
	ble_hids_inp_rep_init_t    input_report_array[1];
	ble_hids_inp_rep_init_t  * p_input_report;
	uint8_t                    hid_info_flags;

	memset((void *)input_report_array, 0, sizeof(ble_hids_inp_rep_init_t));

	//here's a good link desribing usb hid profiles:
	//http://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/
	static uint8_t report_map_data[] = {
  
		// Joystick
		0x05,0x01,			      // USAGE_PAGE (Generic Desktop)
		0x09,0x04,			      // USAGE (Joystick)
		0xa1,0x01,			      // COLLECTION (Application)
		0x85,JOYSTICK_REPORT_ID, //   REPORT_ID (3)

		// 12 Buttons
		0x05,0x09,			      //   USAGE_PAGE (Button)
		0x19,0x01,			      //   USAGE_MINIMUM (Button 1)
		0x29,0x0c,			      //   USAGE_MAXIMUM (Button 12)
		0x15,0x00,			      //   LOGICAL_MINIMUM (0)
		0x25,0x01,			      //   LOGICAL_MAXIMUM (1)
		0x75,0x01,			      //   REPORT_SIZE (1)
		0x95,0x0c,			      //   REPORT_COUNT (12)
		0x55,0x00,			      //   UNIT_EXPONENT (0)
		0x65,0x00,			      //   UNIT (None)
		0x81,0x02,			      //   INPUT (Data,Var,Abs)	
                              
		0xc0				      // END_COLLECTION
	};

	// Initialize HID Service
	p_input_report                      = &input_report_array[JOYSTICK_INPUT_REPORT_KEYS_INDEX];
	p_input_report->max_len             = JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN;
	p_input_report->rep_ref.report_id   = JOYSTICK_INPUT_REP_REF_ID;
	p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&p_input_report->security_mode.cccd_write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&p_input_report->security_mode.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&p_input_report->security_mode.write_perm);


	hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

	memset(&hids_init_obj, 0, sizeof(hids_init_obj));

	hids_init_obj.evt_handler                    = on_hids_evt;
	hids_init_obj.error_handler                  = service_error_handler;
	hids_init_obj.is_kb                          = false;
	hids_init_obj.is_mouse                       = false;
	hids_init_obj.inp_rep_count                  = 1;
	hids_init_obj.p_inp_rep_array                = input_report_array;
	hids_init_obj.outp_rep_count                 = 0;
	hids_init_obj.p_outp_rep_array               = NULL;
	hids_init_obj.feature_rep_count              = 0;
	hids_init_obj.p_feature_rep_array            = NULL;
	hids_init_obj.rep_map.data_len               = sizeof(report_map_data);
	hids_init_obj.rep_map.p_data                 = report_map_data;
	hids_init_obj.hid_information.bcd_hid        = BASE_USB_HID_SPEC_VERSION;
	hids_init_obj.hid_information.b_country_code = 0;
	hids_init_obj.hid_information.flags          = hid_info_flags;
	hids_init_obj.included_services_count        = 0;
	hids_init_obj.p_included_services_array      = NULL;

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.rep_map.security_mode.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hids_init_obj.rep_map.security_mode.write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.hid_information.security_mode.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hids_init_obj.hid_information.security_mode.write_perm);

	//todo: not needed?
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(
	    &hids_init_obj.security_mode_boot_kb_inp_rep.cccd_write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_boot_kb_inp_rep.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hids_init_obj.security_mode_boot_kb_inp_rep.write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_boot_kb_outp_rep.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_boot_kb_outp_rep.write_perm);

	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_protocol.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_protocol.write_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hids_init_obj.security_mode_ctrl_point.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hids_init_obj.security_mode_ctrl_point.write_perm);

	err_code = ble_hids_init(&m_hids, &hids_init_obj);
	APP_ERROR_CHECK(err_code);
}


/**@brief   Function for transmitting a key scan Press & Release Notification.
 *
 * @warning This handler is an example only. You need to analyze how you wish to send the key
 *          release.
 *
 * @param[in]  p_instance     Identifies the service for which Key Notifications are requested.
 * @param[in]  p_key_pattern  Pointer to key pattern.
 * @param[in]  pattern_len    Length of key pattern. 0 < pattern_len < 7.
 * @param[in]  pattern_offset Offset applied to Key Pattern for transmission.
 * @param[out] actual_len     Provides actual length of Key Pattern transmitted, making buffering of
 *                            rest possible if needed.
 * @return     NRF_SUCCESS on success, BLE_ERROR_NO_TX_PACKETS in case transmission could not be
 *             completed due to lack of transmission buffer or other error codes indicating reason
 *             for failure.
 *
 * @note       In case of BLE_ERROR_NO_TX_PACKETS, remaining pattern that could not be transmitted
 *             can be enqueued \ref buffer_enqueue function.
 *             In case a pattern of 'cofFEe' is the p_key_pattern, with pattern_len as 6 and
 *             pattern_offset as 0, the notifications as observed on the peer side would be
 *             1>    'c', 'o', 'f', 'F', 'E', 'e'
 *             2>    -  , 'o', 'f', 'F', 'E', 'e'
 *             3>    -  ,   -, 'f', 'F', 'E', 'e'
 *             4>    -  ,   -,   -, 'F', 'E', 'e'
 *             5>    -  ,   -,   -,   -, 'E', 'e'
 *             6>    -  ,   -,   -,   -,   -, 'e'
 *             7>    -  ,   -,   -,   -,   -,  -
 *             Here, '-' refers to release, 'c' refers to the key character being transmitted.
 *             Therefore 7 notifications will be sent.
 *             In case an offset of 4 was provided, the pattern notifications sent will be from 5-7
 *             will be transmitted.
 */
uint32_t send_key_scan_press_release(ble_hids_t * p_hids,
	uint16_t    * p_key_pattern,
	uint16_t     pattern_len,
	uint16_t     pattern_offset,
	uint16_t   * p_actual_len)
{
	uint32_t err_code;
	uint16_t offset;
	uint16_t data_len;
	uint8_t  data[JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN];

	// HID Report Descriptor enumerates an array of size 6, the pattern hence shall not be any
	// longer than this.
	STATIC_ASSERT((JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN - 2) <= 6);

	ASSERT(pattern_len <= (JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN - 2));

	offset   = pattern_offset;
	data_len = pattern_len;

	do
	{
	    // Reset the data buffer.
		memset(data, 0, sizeof(data));
		
        // Copy the scan code.
		//memcpy(data + SCAN_CODE_POS + offset, p_key_pattern + offset, data_len - offset);		
		memcpy(data, p_key_pattern, data_len);

		if (!m_in_boot_mode){
//			uint8_t my_data[JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN];
//			
//			//12-bit button state into 4 bytes
//			//a,b,x,y
//			my_data[0] = 0x01 & 0xFF;
//			
//			//start, select, lshoulder, rshoulder
//			my_data[1] = 0x00 & 0xFF;
//
//			my_data[2] = 0x00 & 0xFF;
//
//			//left,right,up,down
//			my_data[3] = 0x00 & 0xFF;


			err_code = ble_hids_inp_rep_send(p_hids,
				JOYSTICK_INPUT_REPORT_KEYS_INDEX,
				JOYSTICK_INPUT_REPORT_KEYS_MAX_LEN,
				data);
		}
//		else
//		{
//			err_code = ble_hids_boot_kb_inp_rep_send(p_hids,
//				INPUT_REPORT_KEYS_MAX_LEN,
//				data);
//		}

		if (err_code != NRF_SUCCESS)
		{
			break;
		}

		offset++;
	} while (offset <= data_len);

	*p_actual_len = offset;

	return err_code;
}

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error){
	APP_ERROR_HANDLER(nrf_error);
}
/**@brief Function for sending sample key presses to the peer.
 *
 * @param[in]   key_pattern_len   Pattern length.
 * @param[in]   p_key_pattern     Pattern to be sent.
 */
void keys_send(uint8_t key_pattern_len, uint16_t * p_key_pattern){
	uint32_t err_code;
	uint16_t actual_len;

	err_code = send_key_scan_press_release(&m_hids,
		p_key_pattern,
		key_pattern_len,
		0,
		&actual_len);
	// An additional notification is needed for release of all keys, therefore check
	// is for actual_len <= key_pattern_len and not actual_len < key_pattern_len.
	if ((err_code == BLE_ERROR_NO_TX_PACKETS) && (actual_len <= key_pattern_len)){
	    // Buffer enqueue routine return value is not intentionally checked.
	    // Rationale: Its better to have a a few keys missing than have a system
	    // reset. Recommendation is to work out most optimal value for
	    // MAX_BUFFER_ENTRIES to minimize chances of buffer queue full condition
		UNUSED_VARIABLE(buffer_enqueue(&m_hids, p_key_pattern, key_pattern_len, actual_len));
	}


	if ((err_code != NRF_SUCCESS) &&
	    (err_code != NRF_ERROR_INVALID_STATE) &&
	    (err_code != BLE_ERROR_NO_TX_PACKETS) &&
	    (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)){

		APP_ERROR_HANDLER(err_code);
	}
}
/**@brief Function for handling the HID Report Characteristic Write event.
 *
 * @param[in]   p_evt   HID service event.
 */
static void on_hid_rep_char_write(ble_hids_evt_t * p_evt){
	// The report received is not supported by this application. Do nothing.	
}

/**@brief Function for handling HID events.
 *
 * @details This function will be called for all HID events which are passed to the application.
 *
 * @param[in]   p_hids  HID service structure.
 * @param[in]   p_evt   Event received from the HID service.
 */
static void on_hids_evt(ble_hids_t * p_hids, ble_hids_evt_t * p_evt){
	switch (p_evt->evt_type){
		case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
			m_in_boot_mode = true;
			break;

		case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
			m_in_boot_mode = false;
			break;

		case BLE_HIDS_EVT_REP_CHAR_WRITE:
			on_hid_rep_char_write(p_evt);
			break;

		case BLE_HIDS_EVT_NOTIF_ENABLED:
			break;

		default:
			// No implementation needed.
			break;
		}
}


/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void){
	dis_init();
	bas_init();
	gamepad_hids_init();	
}