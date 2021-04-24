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
#include <asyncsmp.h>

static void _asyncsmp_cb_sem(asyncsmp_req_t *req);
static void _asyncsmp_cb_tn(asyncsmp_req_t *req);
static void _asyncsmp_cb_qmsg(asyncsmp_req_t *req);
static void _asyncsmp_cb_eg(asyncsmp_req_t *req);
static void _asyncsmp_cb_noawait(asyncsmp_req_t *req);
static void _asyncsmp_exec_task(void *args);

asyncsmp_req_t *asyncsmp_req_alloc_custom(asyncsmp_cb_t cb, void *cb_args, size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb = cb;
    req->cb_args = cb_args;
    return req;
}

void asyncsmp_req_free_custom(asyncsmp_req_t *req)
{
    if (req)
    {
        free(req->data);
        free(req);
    }
}

typedef struct asyncsmp_qmsg_args
{
    asyncsmp_enum_t type;
    QueueHandle_t queue;
    SemaphoreHandle_t queue_guard;
} asyncsmp_qmsg_args_t;
asyncsmp_req_t *asyncsmp_req_alloc_qmsg(QueueHandle_t queue, asyncsmp_enum_t msg_type, SemaphoreHandle_t queue_guard, size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb = _asyncsmp_cb_qmsg;
    req->cb_args = malloc(sizeof(asyncsmp_qmsg_args_t));
    if (!req->cb_args)
    {
        free(req->data);
        free(req);
        return NULL;
    }
    ((asyncsmp_qmsg_args_t *)req->cb_args)->type = msg_type;
    ((asyncsmp_qmsg_args_t *)req->cb_args)->queue = queue;
    ((asyncsmp_qmsg_args_t *)req->cb_args)->queue_guard = queue_guard;
    return req;
}

void asyncsmp_req_free_qmsg(asyncsmp_req_t *req)
{
    if (req)
    {
        free(req->cb_args);
        free(req->data);
        free(req);
    }
}

asyncsmp_req_t *asyncsmp_req_alloc_sem(size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb = _asyncsmp_cb_sem;
    req->cb_args = (void *)xSemaphoreCreateBinary();
    if (!req->cb_args)
    {
        free(req->data);
        free(req);
        return NULL;
    }
    return req;
}

bool asyncsmp_await_sem(asyncsmp_req_t *req, TickType_t ticks)
{
    return xSemaphoreTake((SemaphoreHandle_t)req->cb_args, ticks) == pdTRUE;
}

void asyncsmp_req_free_sem(asyncsmp_req_t *req)
{
    if (req)
    {
        vSemaphoreDelete((SemaphoreHandle_t)req->cb_args);
        free(req->data);
        free(req);
    }
}

asyncsmp_req_t *asyncsmp_req_alloc_tn(size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb = _asyncsmp_cb_tn;
    req->cb_args = xTaskGetCurrentTaskHandle();
    return req;
}

bool asyncsmp_await_tn(TickType_t ticks)
{
    return ulTaskNotifyTake(pdTRUE, ticks) != 0;
}

void asyncsmp_req_free_tn(asyncsmp_req_t *req)
{
    if (req)
    {
        free(req->data);
        free(req);
    }
}

typedef struct asyncsmp_eg_args
{
    EventGroupHandle_t eg;
    EventBits_t eb;
} asyncsmp_eg_args_t;
asyncsmp_req_t *asyncsmp_req_alloc_eg(EventGroupHandle_t eg, EventBits_t eb, size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb_args = calloc(1, sizeof(asyncsmp_eg_args_t));
    if (!req->cb_args)
    {
        free(req->data);
        free(req);
        return NULL;
    }
    req->cb = _asyncsmp_cb_eg;
    ((asyncsmp_eg_args_t *)req->cb_args)->eg = eg;
    ((asyncsmp_eg_args_t *)req->cb_args)->eb = eb;
    return req;
}

bool asyncsmp_await_eg_all(EventGroupHandle_t eg, EventBits_t eb, TickType_t ticks)
{
    return xEventGroupWaitBits(eg, eb, pdTRUE, pdTRUE, ticks) == pdTRUE;
}

bool asyncsmp_await_eg_any(EventGroupHandle_t eg, EventBits_t eb, TickType_t ticks)
{
    return xEventGroupWaitBits(eg, eb, pdTRUE, pdFALSE, ticks) == pdTRUE;
}

void asyncsmp_req_free_eg(asyncsmp_req_t *req)
{
    if (req)
    {
        free(req->cb_args);
        free(req->data);
        free(req);
    }
}

asyncsmp_req_t *asyncsmp_req_alloc_noawait(size_t data_size)
{
    asyncsmp_req_t *req = calloc(1, sizeof(asyncsmp_req_t));
    if (!req)
        return NULL;
    if (data_size)
    {
        req->data = calloc(1, data_size);
        if (!req->data)
        {
            free(req);
            return NULL;
        }
    }
    req->cb = _asyncsmp_cb_noawait;
    return req;
}

void asyncsmp_cb(asyncsmp_req_t *req, int8_t ret)
{
    if (req)
    {
        req->ret = ret;
        req->cb(req);
    }
}

typedef struct _asyncsmp_exec_args
{
    asyncsmp_fn_t fn;
    asyncsmp_req_t *req;
} _asyncsmp_exec_args_t;
bool asyncsmp_exec(asyncsmp_fn_t fn, asyncsmp_req_t *req, uint32_t stacksize, uint32_t priority)
{
    _asyncsmp_exec_args_t *args = malloc(sizeof(_asyncsmp_exec_args_t));
    if (!args)
        return false;
    args->fn = fn;
    args->req = req;
    if (xTaskCreate(
            _asyncsmp_exec_task,
            "asyncsmp_exec_task",
            stacksize,
            args,
            priority,
            NULL) != pdTRUE)
    {
        free(args);
        return false;
    }
    return true;
}

/**
 * @brief Internal semaphore request callback function
 */
static void _asyncsmp_cb_sem(asyncsmp_req_t *req)
{
    xSemaphoreGive((SemaphoreHandle_t)req->cb_args);
}

/**
 * @brief Internal task notification request callback function
 */
static void _asyncsmp_cb_tn(asyncsmp_req_t *req)
{
    xTaskNotifyGive((TaskHandle_t)req->cb_args);
}

/**
 * @brief Internal queue message request callback function
 */
static void _asyncsmp_cb_qmsg(asyncsmp_req_t *req)
{
    asyncsmp_msg_t msg = {
        .type = ((asyncsmp_qmsg_args_t *)req->cb_args)->type,
        .data = (void *)req};
    if (((asyncsmp_qmsg_args_t *)req->cb_args)->queue_guard)
    {
        xSemaphoreTake(((asyncsmp_qmsg_args_t *)req->cb_args)->queue_guard,portMAX_DELAY);
        xQueueSendToBack(((asyncsmp_qmsg_args_t *)req->cb_args)->queue, &msg, portMAX_DELAY);
        xSemaphoreGive(((asyncsmp_qmsg_args_t *)req->cb_args)->queue_guard);
        return;
    }
    xQueueSendToBack(((asyncsmp_qmsg_args_t *)req->cb_args)->queue, &msg, portMAX_DELAY);
}

/**
 * @brief Internal event group request callback function
 */
static void _asyncsmp_cb_eg(asyncsmp_req_t *req)
{
    xEventGroupSetBits((EventGroupHandle_t)((asyncsmp_eg_args_t *)req->cb_args)->eg, ((asyncsmp_eg_args_t *)req->cb_args)->eb);
}

/**
 * @brief Internal no-await request callback function
 */
static void _asyncsmp_cb_noawait(asyncsmp_req_t *req)
{
    free(req->data);
    free(req);
}

/**
 * @brief Internal exec task definition
 */
static void _asyncsmp_exec_task(void *args)
{
    ((_asyncsmp_exec_args_t*)args)->fn(((_asyncsmp_exec_args_t*)args)->req);
    free(args);
    vTaskDelete(NULL);
}
