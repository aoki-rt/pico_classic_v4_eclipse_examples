// Copyright 2024 RT Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define LED0 13
#define LED1 14
#define LED2 47
#define LED3 48


void app_main(void)
{
	gpio_reset_pin(LED0);
	gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);
	gpio_reset_pin(LED3);

	gpio_set_direction(LED0, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

    while (true) {
    	gpio_set_level(LED0,1);
    	gpio_set_level(LED1,1);
    	gpio_set_level(LED2,1);
    	gpio_set_level(LED3,1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    	gpio_set_level(LED0,0);
    	gpio_set_level(LED1,0);
    	gpio_set_level(LED2,0);
    	gpio_set_level(LED3,0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
