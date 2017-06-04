#include "my_ble_advertising.h"

pm_peer_id_t m_peer_id;

static void ble_advertising_error_handler(uint32_t nrf_error){
	APP_ERROR_HANDLER(nrf_error);
}


static void on_adv_evt(ble_adv_evt_t ble_adv_evt){
	uint32_t err_code;

	switch (ble_adv_evt){
		case BLE_ADV_EVT_DIRECTED:
			LOG("BLE_ADV_EVT_DIRECTED\r\n");
			break; 

		case BLE_ADV_EVT_FAST:
			LOG("BLE_ADV_EVT_FAST\r\n");
			break; 

		case BLE_ADV_EVT_SLOW:
			LOG("BLE_ADV_EVT_SLOW\r\n");
			break; 

		case BLE_ADV_EVT_FAST_WHITELIST:
			LOG("BLE_ADV_EVT_FAST_WHITELIST\r\n");
			break; 

		case BLE_ADV_EVT_SLOW_WHITELIST:
			LOG("BLE_ADV_EVT_SLOW_WHITELIST\r\n");
			break; 

		case BLE_ADV_EVT_IDLE:
			LOG("BLE_ADV_EVT_IDLE\r\n");
			sleep_mode_enter();
			break; 

		case BLE_ADV_EVT_WHITELIST_REQUEST: {
			LOG("BLE_ADV_EVT_WHITELIST_REQUEST\r\n");
			ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
			ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
			uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
			uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

			err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks, &irk_cnt);
			APP_ERROR_CHECK(err_code);
			LOG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist\r\n", addr_cnt, irk_cnt);

			// Apply the whitelist.
			err_code = ble_advertising_whitelist_reply(whitelist_addrs,	addr_cnt, whitelist_irks, irk_cnt);
			APP_ERROR_CHECK(err_code);
		} break; 

		case BLE_ADV_EVT_PEER_ADDR_REQUEST:{
			LOG("BLE_ADV_EVT_PEER_ADDR_REQUEST\r\n");
			pm_peer_data_bonding_t peer_bonding_data;

			// Only Give peer address if we have a handle to the bonded peer.
			if (m_peer_id != PM_PEER_ID_INVALID){
				err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
				if (err_code != NRF_ERROR_NOT_FOUND){
					APP_ERROR_CHECK(err_code);
					ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_id.id_addr_info);
					err_code = ble_advertising_peer_addr_reply(p_peer_addr);
					APP_ERROR_CHECK(err_code);
				}
			}
		} break;

		default:
			break;
	}
}

void advertising_init(void){
	uint32_t               err_code;
	uint8_t                adv_flags;
	ble_advdata_t          advdata;
	ble_adv_modes_config_t options;

	// Build and set advertising data
	memset(&advdata, 0, sizeof(advdata));

	adv_flags                       = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	advdata.name_type               = BLE_ADVDATA_FULL_NAME;
	advdata.include_appearance      = true;
	advdata.flags                   = adv_flags;
	advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
	advdata.uuids_complete.p_uuids  = m_adv_uuids;

	memset(&options, 0, sizeof(options));
	options.ble_adv_whitelist_enabled      = true;
	options.ble_adv_directed_enabled       = true;
	options.ble_adv_directed_slow_enabled  = false;
	options.ble_adv_directed_slow_interval = 0;
	options.ble_adv_directed_slow_timeout  = 0;
	options.ble_adv_fast_enabled           = true;
	options.ble_adv_fast_interval          = APP_ADV_FAST_INTERVAL;
	options.ble_adv_fast_timeout           = APP_ADV_FAST_TIMEOUT;
	options.ble_adv_slow_enabled           = true;
	options.ble_adv_slow_interval          = APP_ADV_SLOW_INTERVAL;
	options.ble_adv_slow_timeout           = APP_ADV_SLOW_TIMEOUT;

	err_code = ble_advertising_init(&advdata,
		NULL,
		&options,
		on_adv_evt,
		ble_advertising_error_handler);
	APP_ERROR_CHECK(err_code);
}

static void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size){
	pm_peer_id_t peer_id;
	uint32_t     peers_to_copy;

	peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
	                 *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

	peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
	*p_size = 0;

	while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--)){
		p_peers[(*p_size)++] = peer_id;
		peer_id = pm_next_peer_id_get(peer_id);
	}
}


void advertising_start(void){
	ret_code_t ret;

	memset(m_whitelist_peers, PM_PEER_ID_INVALID, sizeof(m_whitelist_peers));
	m_whitelist_peer_cnt = (sizeof(m_whitelist_peers) / sizeof(pm_peer_id_t));

	peer_list_get(m_whitelist_peers, &m_whitelist_peer_cnt);

	ret = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
	APP_ERROR_CHECK(ret);

	// Setup the device identies list.
	// Some SoftDevices do not support this feature.
	ret = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
	if (ret != NRF_ERROR_NOT_SUPPORTED){
		APP_ERROR_CHECK(ret);
	}

	m_is_wl_changed = false;

	ret = ble_advertising_start(BLE_ADV_MODE_FAST);
	APP_ERROR_CHECK(ret);
}
