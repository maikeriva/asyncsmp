/**
 * Copyright 2021 Michele Riva
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <stddef.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asyncsmp_req asyncsmp_req_t;

/**
 * @brief Callback signature
 * 
 * Callback functions must assume this form
 * 
 * @param[in] req Pointer to the called-back request itself
 */
typedef void(*asyncsmp_cb_t)(asyncsmp_req_t *req);

/**
 * @brief Request structure
 * 
 * Requests are the basic element of the asyncsmp architecture
 */
typedef struct asyncsmp_req {
    /**
     * @brief Return code
     * 
     * The return code can be used to determine the outcome
     * of the request. By convention, it is zero or positive if successful, 
     * negative if an error occurred. 
     */
    int8_t ret;
    /**
     * @brief Callback function
     * 
     * This is the function called when a request is marked as complete via asyncsmp_cb().
     * You don't normally need to alter this value.
     */
    asyncsmp_cb_t cb;
    /**
     * @brief Callback arguments
     * 
     * Arguments to be used within the callback function.
     * You don't normally need to alter this value.
     */
    void *cb_args;
    /**
     * @brief Generic data
     * 
     * This pointer is used to attach arbitrary data to the request, most often parameters for the receiver.
     * The memory it points to is automatically deallocated when given as a parameter to an asyncsmp_req_free() function.
     */
    void *data;
    /**
     * @brief Parent request
     * 
     * Requests can be chained together so that tasks do not need to keep 
     * track of their relationship. Refer to the documentation for more details.
     */
    asyncsmp_req_t *parent;
} asyncsmp_req_t;

/**
 * @brief Enum type definition
 * 
 * This must be configured to reflect the 
 * architecture's own enum encoding (unsigned 32bit integer on ESP32)
 */
typedef uint32_t asyncsmp_enum_t;

/**
 * @brief Task message
 * 
 * Messages are used to communicate asynchronously between tasks.
 * Refer to the documentation to understand how to design such tasks.
 */
typedef struct asyncsmp_msg {
    /**
     * @brief Message type
     * 
     * The message type allows the receiving task to discern between 
     * different messages defined in an enumeration.
     */
    asyncsmp_enum_t type;
    /**
     * @brief Message data
     * 
     * Generic pointer often used to carry a request, but also arbitrary data. 
    */
    void *data;
} asyncsmp_msg_t;

/**
 * @brief Asyncronous function signature
 * 
 * Asynchronous functions can be executed via asyncsmp_exec.
 */
typedef void(*asyncsmp_fn_t)(asyncsmp_req_t*);

/**
 * @brief Execute asynchronous function in new task.
 * 
 * This will execute fn in a new task which will be deleted once returned.
 * 
 * @param[in] fn Asynchronous function to execute
 * @param[in] stacksize Stack size of the task created to execute the function
 * @param[in] priority Priority of the task created to execute the function
 * @param[in] req Request to be processed by the function fn
 */
bool asyncsmp_exec(asyncsmp_fn_t fn, asyncsmp_req_t *req, uint32_t stacksize, uint32_t priority);

/**
 * @brief Callback a request with a return code.
 * 
 * After receiving an processing an asyncsmp request, use this function to 
 * callback and signal its completion.
 * 
 * @param[in] req Request to callback
 * @param[in] ret Return code of the operation
 */
void asyncsmp_cb(asyncsmp_req_t *req, int8_t ret);

/**
 * @brief Allocate a custom request.
 * 
 * @param[in] cb Callback function
 * @param[in] cb_args Callback arguments
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_custom(asyncsmp_cb_t cb, void *cb_args, size_t data_size);

/**
 * @brief Free previously allocated custom request.
 * @warning This will also free the data pointer in the request structure
 * @param[out] req Request
*/
void asyncsmp_req_free_custom(asyncsmp_req_t *req);

/**
 * @brief Allocate a queue message request.
 * 
 * @param[in] queue Message queue
 * @param[in] msg_type Message type
 * @param[in] queue_guard Semaphore guard for message queue (optional, can be NULL)
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_qmsg(QueueHandle_t queue, asyncsmp_enum_t msg_type, SemaphoreHandle_t queue_guard, size_t data_size);

/**
 * @brief Free previously allocated queue message request.
 * @warning This will also free the data field in the request structure
 * @param[out] req Request
*/
void asyncsmp_req_free_qmsg(asyncsmp_req_t *req);

/**
 * @brief Allocate a semaphore request.
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_sem(size_t data_size);

/**
 * @brief Await a semaphore request.
 * @param[in] req Request to await for
 * @param[in] ticks Ticks to wait before giving up
 * @return true if request returned within timeout, false otherwise
 */
bool asyncsmp_await_sem(asyncsmp_req_t *req, TickType_t ticks);

/**
 * @brief Free previously allocated semaphore request.
 * @warning This will also free the data field in the request structure
 * @param[out] req Request
*/
void asyncsmp_req_free_sem(asyncsmp_req_t *req);

/**
 * @brief Allocate a task notification request.
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_tn(size_t data_size);

/**
 * @brief Await a semaphore request.
 * @param[in] ticks Ticks to wait before giving up
 * @return true if request returned within timeout, false otherwise
 */
bool asyncsmp_await_tn(TickType_t ticks);

/**
 * @brief Free previously allocated task notification request.
 * @warning This will also free the data field in the request structure
 * @param[out] req Request
*/
void asyncsmp_req_free_tn(asyncsmp_req_t *req);

/**
 * @brief Allocate an event group request.
 * @param[in] eg Event group handle
 * @param[in] eb Event bit (or bitmask) assigned to the request
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_eg(EventGroupHandle_t eg, EventBits_t eb, size_t data_size);

/**
 * @brief Await all of the event group requests in the specified bitmask
 * @param[in] eg Event group handle
 * @param[in] eb Event bit(s) (or bitmask) assigned to the request(s)
 * @param[in] ticks Ticks to wait before giving up
 * @return true if request returned within timeout, false otherwise
 */
bool asyncsmp_await_eg_all(EventGroupHandle_t eg, EventBits_t eb, TickType_t ticks);

/**
 * @brief Await any of the event group requests in the specified bitmask
 * @param[in] eg Event group handle
 * @param[in] eb Event bit(s) (or bitmask) assigned to the request(s)
 * @param[in] ticks Ticks to wait before giving up
 * @return true if request returned within timeout, false otherwise
 */
bool asyncsmp_await_eg_any(EventGroupHandle_t eg, EventBits_t eb, TickType_t ticks);

/**
 * @brief Free previously allocated event group request.
 * @warning This will also free the data field in the request structure
 * @param[out] req Request
*/
void asyncsmp_req_free_eg(asyncsmp_req_t *req);

/**
 * @brief Allocate request without callback.
 * @param[in] data_size Size of data to be carried
 * @return Request, or NULL if allocation failed
 */
asyncsmp_req_t *asyncsmp_req_alloc_noawait(size_t data_size);

#ifdef __cplusplus
}
#endif
