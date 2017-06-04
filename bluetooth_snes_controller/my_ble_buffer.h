#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"

/// <summary>
/// number of elements that can be enqueued
/// </summary>
#define MAX_BUFFER_ENTRIES 5

/// <summary>
/// abstracts buffer element
/// </summary>
typedef struct hid_key_buffer {
	// max data that can be buffered for all entries
	uint8_t      data_offset; /**<  */
	
	// total length of data
	uint8_t      data_len;    /**< Total length of data */
	
	// data
	uint16_t    * p_data; 
	
	// peer and service instance
	ble_hids_t * p_instance; 
} buffer_entry_t;

/// <summary>
/// circular buffer list
/// </summary>
typedef struct{
	// maximum number of entries that can enqueued in the list
	buffer_entry_t buffer[MAX_BUFFER_ENTRIES];

	// read location idx
	uint8_t        rp;   
	
	// write location idx
	uint8_t        wp;   
	
	// elements in list
	uint8_t        count; 
} buffer_list_t;

STATIC_ASSERT(sizeof(buffer_list_t) % 4 == 0);


///////////////////////////////
// buffer queue access macros//
///////////////////////////////


/// <summary>
/// initialization of buffer list
/// </summary>
#define BUFFER_LIST_INIT()     \
    do                         \
    {                          \
        buffer_list.rp    = 0; \
        buffer_list.wp    = 0; \
        buffer_list.count = 0; \
    } while (0)

/// <summary>
/// is buffer list full
/// </summary>
#define BUFFER_LIST_FULL() \
    ((MAX_BUFFER_ENTRIES == buffer_list.count - 1) ? true : false)


/// <summary>
/// is buffer empty
/// </summary>
#define BUFFER_LIST_EMPTY() \
    ((0 == buffer_list.count) ? true : false)

/// <summary>
/// init buffer element
/// </summary>
#define BUFFER_ELEMENT_INIT(i)                 \
    do                                         \
    {                                          \
        buffer_list.buffer[(i)].p_data = NULL; \
    } while (0)


/// <summary>
/// list to enqueue (data, handle, connection, etc...)
/// </summary>
static buffer_list_t buffer_list;

/// <summary>
/// initializing the buffer queue used to key events that could not be transmitted
/// </summary>
void buffer_init(void);

/// <summary>
/// dequeue key presesses that could not be transmitted
/// </summary>
/// <param name="tx_flag">whether the dequeue should result in transmission or not</param>
/// <returns>NRF_SUCCESS on success, else an error code indicating reason for failure</returns>
uint32_t buffer_dequeue(bool tx_flag);

/// <summary>
/// enqueue key presesses that could not be transmitted
/// </summary>
/// <param name="p_hids">the service for which Key Notifications are buffered</param>
/// <param name="p_key_pattern">pointer to key presses</param>
/// <param name="pattern_len">length of key pattern</param>
/// <param name="offset">Offset applied to Key Pattern when requesting a transmission on dequeue </param>
/// <returns>NRF_SUCCESS on success, else an error code indicating reason for failure</returns>
uint32_t buffer_enqueue(ble_hids_t* p_hids, uint16_t* p_key_pattern, uint16_t pattern_len, uint16_t offset);