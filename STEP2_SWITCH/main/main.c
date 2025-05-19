// Copyright 2025 RT Corporation
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

#define SW_L 16
#define SW_C 15
#define SW_R 18

int g_state_r, g_state_c, g_state_l;

void app_main(void)
{
	gpio_reset_pin(LED0);
	gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);
	gpio_reset_pin(LED3);

	gpio_reset_pin(SW_L);
	gpio_reset_pin(SW_C);
	gpio_reset_pin(SW_R);

	gpio_set_direction(LED0, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

	gpio_set_direction(SW_R, GPIO_MODE_INPUT);
	gpio_set_direction(SW_C, GPIO_MODE_INPUT);
	gpio_set_direction(SW_L, GPIO_MODE_INPUT);

	g_state_r = g_state_c = g_state_l = 0;

    while (true) {
    	while (gpio_get_level(SW_L) && gpio_get_level(SW_C) && gpio_get_level(SW_R)) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
    		continue;
    	}
    	if (gpio_get_level(SW_R) == 0) {
    		gpio_set_level(LED0, (++g_state_r) & 0x01);
    	}
    	if (gpio_get_level(SW_C) == 0) {
    		gpio_set_level(LED1, (++g_state_c) & 0x01);
    		gpio_set_level(LED2, (g_state_c) & 0x01);
    	}
		if (gpio_get_level(SW_L) == 0) {
			gpio_set_level(LED3, (++g_state_l) & 0x01);
		}
		
    	vTaskDelay(30 / portTICK_PERIOD_MS);
    	while (!(gpio_get_level(SW_L) && gpio_get_level(SW_C) && gpio_get_level(SW_R))) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
    		continue;
    	}
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}
