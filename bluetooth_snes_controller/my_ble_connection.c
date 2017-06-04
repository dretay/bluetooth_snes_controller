#include "my_ble_connection.h"

/// <summary>
/// uuids of services to advertise
/// </summary>
ble_uuid_t m_adv_uuids[] = { { BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE } };

/// <summary>
/// handle to the current connection
/// </summary>
uint16_t   m_conn_handle  = BLE_CONN_HANDLE_INVALID; 

static void conn_params_error_handler(uint32_t nrf_error){
	APP_ERROR_HANDLER(nrf_error);
}

void conn_params_init(void){
	uint32_t               err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params                  = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
	cp_init.disconnect_on_fail             = false;
	cp_init.evt_handler                    = NULL;
	cp_init.error_handler                  = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}

static void on_ble_evt(ble_evt_t * p_ble_evt) {
	uint32_t err_code;

	switch (p_ble_evt->header.evt_id) {
		case BLE_GAP_EVT_CONNECTED:
			LOG("BLE_GAP_EVT_CONNECTED\r\n");			
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;

		case BLE_EVT_TX_COMPLETE:
			LOG("BLE_EVT_TX_COMPLETE\r\n")
			// can send next key press
			(void) buffer_dequeue(true);
			break; 

		case BLE_GAP_EVT_DISCONNECTED:
			LOG("BLE_GAP_EVT_DISCONNECTED\r\n");
			// Dequeue all keys without transmission.
			(void) buffer_dequeue(false);

			m_conn_handle = BLE_CONN_HANDLE_INVALID;
			
			if (m_is_wl_changed) {
				// The whitelist has been modified, update it in the Peer Manager.
				err_code = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
				APP_ERROR_CHECK(err_code);

				err_code = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
				if (err_code != NRF_ERROR_NOT_SUPPORTED) {
					APP_ERROR_CHECK(err_code);
				}

				m_is_wl_changed = false;
			}
			break;

		case BLE_GATTC_EVT_TIMEOUT:
			// Disconnect on GATT Client timeout event.
			LOG("BLE_GATTC_EVT_TIMEOUT\r\n");
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTS_EVT_TIMEOUT:
			// Disconnect on GATT Server timeout event.
			LOG("BLE_GATTS_EVT_TIMEOUT\r\n");
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break; 

		case BLE_EVT_USER_MEM_REQUEST:
			LOG("BLE_EVT_USER_MEM_REQUEST\r\n")
			err_code = sd_ble_user_mem_reply(m_conn_handle, NULL);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
			LOG("BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST\r\n")
			ble_gatts_evt_rw_authorize_request_t  req;
			ble_gatts_rw_authorize_reply_params_t auth_reply;

			req = p_ble_evt->evt.gatts_evt.params.authorize_request;

			if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID) {
				if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
					(req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
					(req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)) {
					if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
						auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
					}
					else {
						auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
					}
					auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
					err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
						&auth_reply);
					APP_ERROR_CHECK(err_code);
				}
			}
		} break; 

	#if (NRF_SD_BLE_API_VERSION == 3)
		case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
			err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
				NRF_BLE_MAX_MTU_SIZE);
			APP_ERROR_CHECK(err_code);
			break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
	#endif

		default:
			break;
		}
}


static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
    //The Connection state module has to be fed BLE events in order to function correctly
    //Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions.

	ble_conn_state_on_ble_evt(p_ble_evt);
	pm_on_ble_evt(p_ble_evt);
//	bsp_btn_ble_on_ble_evt(p_ble_evt);
	on_ble_evt(p_ble_evt);
	ble_advertising_on_ble_evt(p_ble_evt);
	ble_conn_params_on_ble_evt(p_ble_evt);
	ble_hids_on_ble_evt(&m_hids, p_ble_evt);
	ble_bas_on_ble_evt(&m_bas, p_ble_evt);
}

static void sys_evt_dispatch(uint32_t sys_evt) {
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
	fs_sys_event_handler(sys_evt);

	// Dispatch to the Advertising module last, since it will check if there are any
	// pending flash operations in fstorage. Let fstorage process system events first,
	// so that it can report correctly to the Advertising module.
	ble_advertising_on_sys_evt(sys_evt);
}

void ble_stack_init(void) {
	uint32_t err_code;

	// Initialize the SoftDevice handler module.
	SOFTDEVICE_HANDLER_APPSH_INIT(NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM, true);

	ble_enable_params_t ble_enable_params;
	err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
		PERIPHERAL_LINK_COUNT,
		&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	// Check the ram settings against the used number of links
	CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

	// Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
	ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
	err_code = softdevice_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Register with the SoftDevice handler module for BLE events.
	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}
