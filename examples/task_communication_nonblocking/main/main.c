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
#include <esp_log.h>

// Define task 2 (receiver) and its queue
QueueHandle_t task2_queue;
void task2(void *args)
{
    asyncsmp_req_t *req;
    while (true)
    {
        // Wait for requests
        xQueueReceive(task2_queue, &req, portMAX_DELAY);

        // Do some stuff
        ESP_LOGI("TASK 2", "Received request, doing some stuff, then responding");
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Callback with code 0 when done
        asyncsmp_cb(req, 0);
    }
}

// Define task 1 (sender/receiver, non-blocking), its message types, and its queue
typedef enum
{
    TASK1_SENDREQUEST,
    TASK1_PROCESSRESPONSE
} task1_msg_t;
QueueHandle_t task1_queue;
void task1(void *args)
{
    asyncsmp_msg_t msg = {};
    while (true)
    {
        // Wait for messages
        xQueueReceive(task1_queue, &msg, portMAX_DELAY);

        switch (msg.type)
        {
        case TASK1_SENDREQUEST:
        {
            asyncsmp_req_t *req = (asyncsmp_req_t *)msg.data;

            // Allocate and send new request to task 2
            ESP_LOGI("TASK 1", "Sending new request to task 2 (I am not blocking new requests)");
            asyncsmp_req_t *newreq = asyncsmp_req_alloc_qmsg(task1_queue, TASK1_PROCESSRESPONSE, NULL, 0);
            xQueueSendToBack(task2_queue, &newreq, portMAX_DELAY);

            // Callback with code 0 when done
            asyncsmp_cb(req, 0);

            // Break and wait for next message
            break;
        }
        case TASK1_PROCESSRESPONSE:
        {
            asyncsmp_req_t *req = (asyncsmp_req_t *)msg.data;

            // Received response from task 2, free resources
            ESP_LOGI("TASK 1", "Received response from task 2 (result:%d)", req->ret);
            asyncsmp_req_free_qmsg(req);

            // Break and wait for next message
            break;
        }
        default:
            ESP_LOGE("TASK 1", "Unknown message type");
        }
    }
}

// Main program execution
void app_main(void)
{
    // Initialize task2 (receiver)
    task2_queue = xQueueCreate(5, sizeof(asyncsmp_req_t));
    xTaskCreate(task2, "task2", 2048, NULL, 1, NULL);

    // Initialize task1 (sender/receiver, non-blocking)
    task1_queue = xQueueCreate(5, sizeof(asyncsmp_msg_t));
    xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);

    // Send a request to task 1
    ESP_LOGI("APP_MAIN", "Sending message to task1");
    asyncsmp_req_t *req1 = asyncsmp_req_alloc_noawait(0);
    asyncsmp_msg_t msg1 = {
        .type = TASK1_SENDREQUEST,
        .data = req1};
    xQueueSendToBack(task1_queue, &msg1, portMAX_DELAY);

    // Send another request to task 1, even if it is waiting response from task 2
    ESP_LOGI("APP_MAIN", "Sending another message to task1");
    asyncsmp_req_t *req2 = asyncsmp_req_alloc_noawait(0);
    asyncsmp_msg_t msg2 = {
        .type = TASK1_SENDREQUEST,
        .data = req2};
    xQueueSendToBack(task1_queue, &msg2, portMAX_DELAY);
}
