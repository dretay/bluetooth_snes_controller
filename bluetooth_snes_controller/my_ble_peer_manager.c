#include "my_ble_peer_manager.h"

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
void peer_manager_init(bool erase_bonds){
	ble_gap_sec_params_t sec_param;
	ret_code_t           err_code;

	err_code = pm_init();
	APP_ERROR_CHECK(err_code);

	if (erase_bonds){
		err_code = pm_peers_delete();
		APP_ERROR_CHECK(err_code);
	}

	memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

	// Security parameters to be used for all security procedures.
	sec_param.bond           = SEC_PARAM_BOND;
	sec_param.mitm           = SEC_PARAM_MITM;
	sec_param.lesc           = SEC_PARAM_LESC;
	sec_param.keypress       = SEC_PARAM_KEYPRESS;
	sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
	sec_param.oob            = SEC_PARAM_OOB;
	sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
	sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
	sec_param.kdist_own.enc  = 1;
	sec_param.kdist_own.id   = 1;
	sec_param.kdist_peer.enc = 1;
	sec_param.kdist_peer.id  = 1;

	err_code = pm_sec_params_set(&sec_param);
	APP_ERROR_CHECK(err_code);

	err_code = pm_register(pm_evt_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt){
	ret_code_t err_code;

	switch (p_evt->evt_id){
		case PM_EVT_BONDED_PEER_CONNECTED:
			{
				LOG("Connected to previously bonded device\r\n");
				m_peer_id = p_evt->peer_id;
				err_code  = pm_peer_rank_highest(p_evt->peer_id);
				if (err_code != NRF_ERROR_BUSY)
				{
					APP_ERROR_CHECK(err_code);
				}
			} break;

		case PM_EVT_CONN_SEC_START:
			break;

		case PM_EVT_CONN_SEC_SUCCEEDED:
			{
				LOG("Link secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
					ble_conn_state_role(p_evt->conn_handle),
					p_evt->conn_handle,
					p_evt->params.conn_sec_succeeded.procedure);
				m_peer_id = p_evt->peer_id;
				err_code  = pm_peer_rank_highest(p_evt->peer_id);
				if (err_code != NRF_ERROR_BUSY)
				{
					APP_ERROR_CHECK(err_code);
				}
				if (p_evt->params.conn_sec_succeeded.procedure == PM_LINK_SECURED_PROCEDURE_BONDING)
				{
					LOG("New Bond, add the peer to the whitelist if possible\r\n");
					LOG("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d\r\n",
						m_whitelist_peer_cnt + 1,
						BLE_GAP_WHITELIST_ADDR_MAX_COUNT);
					if (m_whitelist_peer_cnt < BLE_GAP_WHITELIST_ADDR_MAX_COUNT)
					{
						//bonded to a new peer, add it to the whitelist.
						m_whitelist_peers[m_whitelist_peer_cnt++] = m_peer_id;
						m_is_wl_changed = true;
					}
					//Note: This code will use the older bonded device in the white list and not add any newer bonded to it
					//      You should check on what kind of white list policy your application should use.
				}
			} break;

		case PM_EVT_CONN_SEC_FAILED:
			{
				/** In some cases, when securing fails, it can be restarted directly. Sometimes it can
				 *  be restarted, but only after changing some Security Parameters. Sometimes, it cannot
				 *  be restarted until the link is disconnected and reconnected. Sometimes it is
				 *  impossible, to secure the link, or the peer device does not support it. How to
				 *  handle this error is highly application dependent. */
				switch (p_evt->params.conn_sec_failed.error)
				{
				case PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING:
					// Rebond if one party has lost its keys.
					err_code = pm_conn_secure(p_evt->conn_handle, true);
					if (err_code != NRF_ERROR_INVALID_STATE)
					{
						APP_ERROR_CHECK(err_code);
					}
					break; // PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING

				default:
					break;
				}
			} break;

		case PM_EVT_CONN_SEC_CONFIG_REQ:
			{
				// Reject pairing request from an already bonded peer.
				pm_conn_sec_config_t conn_sec_config = { .allow_repairing = false };
				pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
			} break;

		case PM_EVT_STORAGE_FULL:
			{
				// Run garbage collection on the flash.
				err_code = fds_gc();
				if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
				{
					// Retry.
				}
				else
				{
					APP_ERROR_CHECK(err_code);
				}
			} break;

		case PM_EVT_ERROR_UNEXPECTED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
			break;

		case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
			break;

		case PM_EVT_PEER_DATA_UPDATE_FAILED:
			// Assert.
			APP_ERROR_CHECK_BOOL(false);
			break;

		case PM_EVT_PEER_DELETE_SUCCEEDED:
			break;

		case PM_EVT_PEER_DELETE_FAILED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
			break;

		case PM_EVT_PEERS_DELETE_SUCCEEDED:
			advertising_start();
			break;

		case PM_EVT_PEERS_DELETE_FAILED:
			// Assert.
			APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
			break;

		case PM_EVT_LOCAL_DB_CACHE_APPLIED:
			break;

		case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
			// The local database has likely changed, send service changed indications.
			pm_local_database_has_changed();
			break;

		case PM_EVT_SERVICE_CHANGED_IND_SENT:
		case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
			break;

		default:
			// No implementation needed.
			break;
	}
}