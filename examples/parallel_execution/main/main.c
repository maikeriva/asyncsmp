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

// Function to execute asynchronously
void do_stuff(asyncsmp_req_t *req)
{
    // Do something
    ESP_LOGI("DO_STUFF", "Yay! I am doing stuff asynchronously");

    // Callback when done
    asyncsmp_cb(req, 0);
}

// Main program execution
void app_main()
{
    // Allocate async request
    asyncsmp_req_t *req = asyncsmp_req_alloc_sem(0);

    // Execute function asynchronously in a new task
    ESP_LOGI("APP_MAIN", "Executing function in a new task");
    asyncsmp_exec(do_stuff, req, 2048, 1);

    // Do something else while waiting for response (optional)
    ESP_LOGI("APP_MAIN", "Doing something else while waiting for response");

    // Await until the async operation completes
    asyncsmp_await_sem(req, portMAX_DELAY);
    ESP_LOGI("APP_MAIN", "Async parallel execution complete (result: %d)", req->ret);

    // Done, free resources
    asyncsmp_req_free_sem(req);
}
