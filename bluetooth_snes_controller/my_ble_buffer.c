#include "my_ble_buffer.h"

uint8_t m_sample_key_press_scan_str[] = /**< Key pattern to be sent when the key press button has been pushed. */
{
	0x0b,                                      /* Key h */
	0x08,                                      /* Key e */
	0x0f,                                      /* Key l */
	0x0f,                                      /* Key l */
	0x12,                                      /* Key o */
	0x28                                       /* Key Return */
};

uint8_t m_caps_on_key_scan_str[] = /**< Key pattern to be sent when the output report has been written with the CAPS LOCK bit set. */
{
	0x06,                                 /* Key C */
	0x04,                                 /* Key a */
	0x13,                                 /* Key p */
	0x16,                                 /* Key s */
	0x12,                                 /* Key o */
	0x11,                                 /* Key n */
};

uint8_t m_caps_off_key_scan_str[] = /**< Key pattern to be sent when the output report has been written with the CAPS LOCK bit cleared. */
{
	0x06,                                  /* Key C */
	0x04,                                  /* Key a */
	0x13,                                  /* Key p */
	0x16,                                  /* Key s */
	0x12,                                  /* Key o */
	0x09,                                  /* Key f */
};


/**@brief   Function for initializing the buffer queue used to key events that could not be
 *          transmitted
 *
 * @warning This handler is an example only. You need to analyze how you wish to buffer or buffer at
 *          all.
 *
 * @note    In case of HID keyboard, a temporary buffering could be employed to handle scenarios
 *          where encryption is not yet enabled or there was a momentary link loss or there were no
 *          Transmit buffers.
 */
void buffer_init(void){
	uint32_t buffer_count;


	BUFFER_LIST_INIT();

	for (buffer_count = 0; buffer_count < MAX_BUFFER_ENTRIES; buffer_count++){
		BUFFER_ELEMENT_INIT(buffer_count);
	}
}


/**@brief Function for enqueuing key scan patterns that could not be transmitted either completely
 *        or partially.
 *
 * @warning This handler is an example only. You need to analyze how you wish to send the key
 *          release.
 *
 * @param[in]  p_hids         Identifies the service for which Key Notifications are buffered.
 * @param[in]  p_key_pattern  Pointer to key pattern.
 * @param[in]  pattern_len    Length of key pattern.
 * @param[in]  offset         Offset applied to Key Pattern when requesting a transmission on
 *                            dequeue, @ref buffer_dequeue.
 * @return     NRF_SUCCESS on success, else an error code indicating reason for failure.
 */
uint32_t buffer_enqueue(ble_hids_t * p_hids,
	uint16_t    * p_key_pattern,
	uint16_t     pattern_len,
	uint16_t     offset){

	buffer_entry_t * element;
	uint32_t         err_code = NRF_SUCCESS;

	if (BUFFER_LIST_FULL()){
	    // Element cannot be buffered.
		err_code = NRF_ERROR_NO_MEM;
		LOG("my_ble_buffer: buffer list full\n");
	}
	else{
	    // Make entry of buffer element and copy data.
		element              = &buffer_list.buffer[(buffer_list.wp)];
		element->p_instance  = p_hids;
		element->p_data      = p_key_pattern;
		element->data_offset = offset;
		element->data_len    = pattern_len;

		buffer_list.count++;
		buffer_list.wp++;

		if (buffer_list.wp == MAX_BUFFER_ENTRIES){
			buffer_list.wp = 0;
		}
	}

	return err_code;
}


/**@brief   Function to dequeue key scan patterns that could not be transmitted either completely of
 *          partially.
 *
 * @warning This handler is an example only. You need to analyze how you wish to send the key
 *          release.
 *
 * @param[in]  tx_flag   Indicative of whether the dequeue should result in transmission or not.
 * @note       A typical example when all keys are dequeued with transmission is when link is
 *             disconnected.
 *
 * @return     NRF_SUCCESS on success, else an error code indicating reason for failure.
 */
uint32_t buffer_dequeue(bool tx_flag){
	buffer_entry_t * p_element;
	uint32_t         err_code = NRF_SUCCESS;
	uint16_t         actual_len;

	if (BUFFER_LIST_EMPTY()){
		err_code = NRF_ERROR_NOT_FOUND;
	}
	else{
		bool remove_element = true;

		p_element = &buffer_list.buffer[(buffer_list.rp)];

		if (tx_flag){
			err_code = send_key_scan_press_release(p_element->p_instance,
				p_element->p_data,
				p_element->data_len,
				p_element->data_offset,
				&actual_len);
			// An additional notification is needed for release of all keys, therefore check
			// is for actual_len <= element->data_len and not actual_len < element->data_len
			if ((err_code == BLE_ERROR_NO_TX_PACKETS) && (actual_len <= p_element->data_len)){
			    // Transmission could not be completed, do not remove the entry, adjust next data to
			    // be transmitted
				p_element->data_offset = actual_len;
				remove_element         = false;
			}
		}

		if (remove_element){
			BUFFER_ELEMENT_INIT(buffer_list.rp);

			buffer_list.rp++;
			buffer_list.count--;

			if (buffer_list.rp == MAX_BUFFER_ENTRIES){
				buffer_list.rp = 0;
			}
		}
	}

	return err_code;
}