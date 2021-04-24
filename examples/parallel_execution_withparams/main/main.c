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

// Define function to execute asynchronously and parameters structure
typedef struct do_stuff_params
{
    char *param1;
    int32_t param2;
} do_stuff_params_t;
void do_stuff(asyncsmp_req_t *req)
{
    // Do something with parameters
    do_stuff_params_t *params = (do_stuff_params_t *)req->data;
    ESP_LOGI("DO_STUFF", "Yay! I am doing stuff with parameters (param1:%s param2:%d)", params->param1, params->param2);

    // Callback when done
    asyncsmp_cb(req, 0);
}

// Main program execution
void app_main()
{
    // Allocate async request
    asyncsmp_req_t *req = asyncsmp_req_alloc_sem(sizeof(do_stuff_params_t));

    // Fill in function parameters
    do_stuff_params_t *params = (do_stuff_params_t *)req->data;
    params->param1 = "stringparam";
    params->param2 = 15;

    // Execute function asynchronously in a new task
    ESP_LOGI("APP_MAIN", "Executing function in a new task (param1:%s param2:%d)", params->param1, params->param2);
    asyncsmp_exec(do_stuff, req, 2048, 1);

    // Do something else while the async operation is in progress
    ESP_LOGI("APP_MAIN", "Doing something else while waiting for response");

    // Await until the async operation completes
    asyncsmp_await_sem(req, portMAX_DELAY);
    ESP_LOGI("APP_MAIN", "Async parallel execution complete (result:%d param1:%s param2:%d)", req->ret, params->param1, params->param2);

    // Done, free resources
    asyncsmp_req_free_sem(req);
}
