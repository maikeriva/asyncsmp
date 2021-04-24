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

// Define task1 (receiver), its message types, and its queue
typedef enum
{
    TASK1_MESSAGE1,
    TASK1_MESSAGE2
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
        case TASK1_MESSAGE1:
        {
            asyncsmp_req_t *req = (asyncsmp_req_t *)msg.data;

            // Do some kind of stuff
            ESP_LOGI("TASK 1","Received message 1, doing some kind of stuff, then responding");
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Callback with code 0 when done
            asyncsmp_cb(req, 0);

            // Break and wait for next message
            break;
        }
        case TASK1_MESSAGE2:
        {
            asyncsmp_req_t *req = (asyncsmp_req_t *)msg.data;

            // Do some other kind of stuff
            ESP_LOGI("TASK 1","Received message 2, doing some other kind of stuff, then responding");
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Callback with code 0 when done
            asyncsmp_cb(req, 0);

            // Break and wait for next message
            break;
        }
        default:
            ESP_LOGE("TASK 1","Unknown message type");
        }
    }
}

// Main program execution
void app_main(void)
{
    // Initialize task1 (receiver)
    task1_queue = xQueueCreate(5, sizeof(asyncsmp_msg_t));
    xTaskCreate(task1, "task1", 1024, NULL, 1, NULL);

    // Send message 1
    ESP_LOGI("APP_MAIN","Sending message type 1");
    asyncsmp_req_t *req1 = asyncsmp_req_alloc_sem(0);
    asyncsmp_msg_t msg1 = {
        .type = TASK1_MESSAGE1,
        .data = req1};
    xQueueSendToBack(task1_queue, &msg1, portMAX_DELAY);

    // Send message 2
    ESP_LOGI("APP_MAIN","Sending message type 2");
    asyncsmp_req_t *req2 = asyncsmp_req_alloc_sem(0);
    asyncsmp_msg_t msg2 = {
        .type = TASK1_MESSAGE2,
        .data = req2};
    xQueueSendToBack(task1_queue, &msg2, portMAX_DELAY);

    // Do something else while waiting for message 1 response
    ESP_LOGI("APP_MAIN","Doing something else while waiting for message 1 response");

    // Await for message 1 response
    asyncsmp_await_sem(req1, portMAX_DELAY);
    ESP_LOGI("APP_MAIN","Received message 1 response (result:%d)", req1->ret);
    asyncsmp_req_free_sem(req1);

    // Do something else while waiting for message 2 response
    ESP_LOGI("APP_MAIN","Doing something else while waiting for message 2 response");

    // Await for message 2 response
    asyncsmp_await_sem(req2, portMAX_DELAY);
    ESP_LOGI("APP_MAIN","Received message 2 response (result:%d)", req1->ret);
    asyncsmp_req_free_sem(req2);
}
