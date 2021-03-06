// _BUFFER_H_ is a flag used to prevent buffer.h from being included more than
// once in any given source file.
#ifndef _ITEMBUFFER_H_
#define _ITEMBUFFER_H_
#include <stdint.h> // include defs for uint16_t etc.
#include <stdbool.h>

/** @file
 * @defgroup item_buffer FIFO Item Buffer
 * @ingroup data_strucutre
 *
 * @author Anthony Merlino
 *
 * @{
 */

/** The FIFO buffer can buffer any data type, from a char to a complex 
* structure. Define item_t to be the type of data to buffer. Default is
* char, it is suggested to override this in system.h if desired.
*
* Note: only one data type can be defined per build. If two different data
* type buffers are required in a single project you can make copies of 
* buffer.h and buffer.c or update the module to track the item size per
* buffer.
*/

/** data structure to hold the required information for each buffer
*/
typedef struct {
	uint16_t size; /**< size is the number of items in the buffer >*/
        uint8_t item_size; /**<item size is the number of words in the item type>*/
	uint16_t *front; /**< pointer to first item in buffer >*/
	uint16_t *rear; /**< pointer to next open position in the buffer >*/
	uint16_t *buffer_start; /**< buffer start location in memory >*/
	uint16_t *buffer_end; /**< buffer end location in memory >*/
	void (*Callback)(void * buf); /**< Push callback, useful if buffer is used
		for communications, does not need to be used/set, initializes to 0>*/
} item_buffer_t;

/** Push will add one item, data, to the FIFO buffer
 * 
 * Push will add one item to the rear of the data buffer then increment (and
 * wrap is needed) the rear. If the buffer is full it will overwrite the data
 * at the front of the buffer and increment the front.
 * 
 * BufferInit() must be used to initialize the buffer prior to calling Push and
 * passing it a pointer to the buffer.
 * 
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @param data item_t data to be added to the rear of the FIFO buffer
 * 
 * @warning Push is not yet interrupt safe
 */
bool PushItem(item_buffer_t *buffer, uint16_t *data);

/** Pop will return one item from the front of the FIFO buffer
 * 
 * Pop will return the item at the front of the FIFO buffer then increment (and
 * wrap as needed) the front. If the buffer is empty it will return 0.
 * 
 * BufferInit() must be used to initialize the buffer prior to calling Pop and
 * passing it a pointer to the buffer.
 * 
 * @param [in] buffer Pointer to the buffer_t data structure holding the buffer info
 * @param [out] data Data from the front of the buffer copied to the location pointed to
 * 
 * @warning Pop is not yet interrupt safe
 */
void PopItem(item_buffer_t *buffer, uint16_t *data);

/** GetSize returns the number of items in the FIFO buffer
 * 
 * BufferInit() should be used to initialize the buffer otherwise the return
 * value will be meaningless
 * 
 * @param buffer Pointer to the buffer_t data structure holding the buffer info
 * @return Number of items in the buffer
 */
uint16_t ItemBufferGetSize(item_buffer_t *buffer);

/** Initialize a FIFO buffer
 * 
 * Example code:
 * @code
 * #define TX_BUFFER_LENGTH 512
 * buffer_t tx; // transmit buffer
 * item_t tx_buffer_array[TX_BUFFER_LENGTH]
 * ...
 * BufferInit(&tx, &tx_buffer_array[0], TX_BUFFER_LENGTH);
 * @endcode
 * 
 * @param buffer Pointer to the buffer_t data structure to be initialized
 * @param data_array Array of item_t data to implement the actual buffer
 * @param max_size Maximum size of the buffer (should be the same length as the
 * array)
 * @param type_size Size of the item
 */
void ItemBufferInit(item_buffer_t *buffer, uint16_t *data_array, uint16_t max_size, uint8_t type_size);

/** Set Callback function for buffer to be called after items are Push'd to the buffer
 *
 * The callback function will be called after anything is Push'd to
 * the buffer. The function will be called with a pointer to the buffer which had an item pushed
 * onto it.
 *
 * Example:
 * @code
 * void TxCallback(buffer_t * buf);
 * #define TX_BUFFER_LENGTH 512
 * buffer_t tx; // transmit buffer
 * item_t tx_buffer_array[TX_BUFFER_LENGTH]
 * ...
 * BufferInit(&tx, &tx_buffer_array[0], TX_BUFFER_LENGTH);
 * BufferSetCallback(&tx, TxCallback);
 * ...
 * void TxCallback(buffer_t * buf) {
 * 		SET_UART_TX_IE();
 * }
 * @endcode
 * This example is useful for a uC which has a hardware Tx interrupt flag which is set
 * whenever there is room in the hardware Tx FIFO buffer. When done transmitting the
 * interrupt must be disabled to avoid getting stuck in the ISR. When data needs to be
 * sent the interrupt must be enabled again, thus the need for the callback.
 *
 * Another usage could be to handle received data on a receive buffer.
 *
 * @param buffer Pointer to the buffer_t data structure whose callback function is to be set
 * @param Callback Function pointer to a callback function with no return value  and a
 * 	buffer_t pointer input.
 */
void ItemBufferSetCallback(item_buffer_t * buffer, void (*Callback)(item_buffer_t * buffer));

/** Clear/remove the callback function for 'buffer'
 *
 * @param buffer Pointer to the buffer_t data structure whose callback function is to be cleared
 */
void ItemBufferClearCallback(item_buffer_t * buffer);

/** @} */

#endif // _ITEMBUFFER_H_
