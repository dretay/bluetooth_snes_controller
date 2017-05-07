#pragma once

#include "my_ble_libs.h"
#include "my_ble_globals.h"


#define MAX_BUFFER_ENTRIES               5                                           /**< Number of elements that can be enqueued */

/** Abstracts buffer element */
typedef struct hid_key_buffer
{
	uint8_t      data_offset; /**< Max Data that can be buffered for all entries */
	uint8_t      data_len;    /**< Total length of data */
	uint16_t    * p_data;      /**< Scanned key pattern */
	ble_hids_t * p_instance;  /**< Identifies peer and service instance */
} buffer_entry_t;
/** Circular buffer list */
typedef struct
{
	buffer_entry_t buffer[MAX_BUFFER_ENTRIES]; /**< Maximum number of entries that can enqueued in the list */
	uint8_t        rp;                         /**< Index to the read location */
	uint8_t        wp;                         /**< Index to write location */
	uint8_t        count;                      /**< Number of elements in the list */
} buffer_list_t;

STATIC_ASSERT(sizeof(buffer_list_t) % 4 == 0);
/**Buffer queue access macros
 *
 * @{ */
/** Initialization of buffer list */
#define BUFFER_LIST_INIT()     \
    do                         \
    {                          \
        buffer_list.rp    = 0; \
        buffer_list.wp    = 0; \
        buffer_list.count = 0; \
    } while (0)

/** Provide status of data list is full or not */
#define BUFFER_LIST_FULL() \
    ((MAX_BUFFER_ENTRIES == buffer_list.count - 1) ? true : false)

/** Provides status of buffer list is empty or not */
#define BUFFER_LIST_EMPTY() \
    ((0 == buffer_list.count) ? true : false)

#define BUFFER_ELEMENT_INIT(i)                 \
    do                                         \
    {                                          \
        buffer_list.buffer[(i)].p_data = NULL; \
    } while (0)

/** List to enqueue not just data to be sent, but also related information like the handle, connection handle etc */
static buffer_list_t buffer_list;


void buffer_init(void);
uint32_t buffer_dequeue(bool tx_flag);
uint32_t buffer_enqueue(ble_hids_t*, uint16_t*, uint16_t, uint16_t);