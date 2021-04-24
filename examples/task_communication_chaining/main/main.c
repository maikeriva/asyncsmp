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

// Define task2 (receiver) and its queue
QueueHandle_t task2_queue;
void task2(void *args)
{
    asyncsmp_req_t *req;
    while (true)
    {
        // Wait for requests
        xQueueReceive(task2_queue, &req, portMAX_DELAY);

        // Do some stuff
        ESP_LOGI("TASK 2","Received request, doing some stuff, then responding");
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Callback with code 0 when done
        asyncsmp_cb(req, 0);
    }
}

// Define task1 (sender/receiver, non-blocking), its message types and its queue
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

            // Received request, allocate child request for task2
            ESP_LOGI("TASK 1","Received request, sending child request to task 2"); 
            asyncsmp_req_t *childreq = asyncsmp_req_alloc_qmsg(task1_queue, TASK1_PROCESSRESPONSE, NULL, 0);
            
            // Chain it with current request
            childreq->parent = req;

            // Send child request to task2
            xQueueSendToBack(task2_queue, &childreq, portMAX_DELAY);

            // Do something while waiting for task 2 response (optional)
            ESP_LOGI("TASK 1","Doing something else while waiting for response from task 2"); 

            // Break and wait for next message
            break;
        }
        case TASK1_PROCESSRESPONSE:
        {
            asyncsmp_req_t *req = (asyncsmp_req_t *)msg.data;

            // Respond to parent request
            ESP_LOGI("TASK 1","Received response from task 2 (result:%d), responding to parent request", req->ret);
            asyncsmp_cb(req->parent,0);

            // Free resources of child request
            asyncsmp_req_free_qmsg(req);

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
    // Initialize task 2
    task2_queue = xQueueCreate(5, sizeof(asyncsmp_req_t));
    xTaskCreate(task2, "task2", 2048, NULL, 1, NULL);

    // Initialize task 1
    task1_queue = xQueueCreate(5, sizeof(asyncsmp_msg_t));
    xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);

    // Send request to task 1
    ESP_LOGI("APP_MAIN","Sending request to task 1");
    asyncsmp_req_t *req = asyncsmp_req_alloc_sem(0);
    asyncsmp_msg_t msg = {
        .type = TASK1_SENDREQUEST,
        .data = req};
    xQueueSendToBack(task1_queue,&msg,portMAX_DELAY);

    // Do something while waiting for response
    ESP_LOGI("APP_MAIN","Doing something while waiting for response from task 1");

    // Wait for response
    asyncsmp_await_sem(req,portMAX_DELAY);
    ESP_LOGI("APP_MAIN","Received response from task 1 (result:%d)",req->ret);
    asyncsmp_req_free_sem(req);
}
