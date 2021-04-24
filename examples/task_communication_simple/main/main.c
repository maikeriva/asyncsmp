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

// Define task1 (receiver) and its queue
QueueHandle_t task1_queue;
void task1(void *args)
{
    asyncsmp_req_t *req;
    while (true)
    {
        // Wait for requests
        xQueueReceive(task1_queue, &req, portMAX_DELAY);

        // Request received, do something
        ESP_LOGI("TASK 1","Received request, doing some stuff, then responding");
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Callback when done
        asyncsmp_cb(req, 0);
    }
}

// Main control task
void app_main(void)
{
    // Initialize task1 (receiver)
    task1_queue = xQueueCreate(5, sizeof(asyncsmp_req_t));
    xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);

    // Send a request
    ESP_LOGI("APP_MAIN","Sending request 1");
    asyncsmp_req_t *req1 = asyncsmp_req_alloc_sem(0);
    xQueueSendToBack(task1_queue,&req1,portMAX_DELAY);

    // Send another request of different type
    ESP_LOGI("APP_MAIN","Sending request 2 (different type)");
    asyncsmp_req_t *req2 = asyncsmp_req_alloc_tn(0);
    xQueueSendToBack(task1_queue,&req2,portMAX_DELAY);

    // Do something while waiting for response from request 1
    ESP_LOGI("APP_MAIN","Doing something while waiting for response from request 1");

    // Wait for response from request 1
    asyncsmp_await_sem(req1,portMAX_DELAY);
    ESP_LOGI("APP_MAIN","Received response from request 1 (result:%d)",req1->ret);
    asyncsmp_req_free_sem(req1);

    // Do something while waiting for response from request 2
    ESP_LOGI("APP_MAIN","Doing something while waiting for response from request 2");

    // Wait for response from request 2
    asyncsmp_await_tn(portMAX_DELAY);
    ESP_LOGI("APP_MAIN","Received response from request 2 (result:%d)",req2->ret);
    asyncsmp_req_free_tn(req2);
}
