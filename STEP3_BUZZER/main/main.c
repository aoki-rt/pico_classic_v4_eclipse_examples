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
#include "driver/ledc.h"

#define LED0 13
#define LED1 14
#define LED2 47
#define LED3 48

#define SW_L 16
#define SW_C 15
#define SW_R 18

#define BUZZER 38

#define INC_FREQ 2000
#define DEC_FREQ 1000

#define FREQ_C 523	//C
#define FREQ_D 587	//D
#define FREQ_E 659	//E


char	g_mode;

void ledSet(char data)
{
	if(data & 0x01){
		gpio_set_level(LED0,1);
	}else{
		gpio_set_level(LED0,0);
	}
	if(data & 0x02){
		gpio_set_level(LED1,1);
	}else{
		gpio_set_level(LED1,0);
	}
	if(data & 0x04){
		gpio_set_level(LED2,1);
	}else{
		gpio_set_level(LED2,0);
	}
	if(data & 0x08){
		gpio_set_level(LED3,1);
	}else{
		gpio_set_level(LED3,0);
	}
}

void modeExec(char mode)
{
	switch(mode){
	case 1:
		ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, FREQ_C);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,512);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		break;
	case 2:
		ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, FREQ_D);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		break;
	case 3:
		ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, FREQ_E);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,1024);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		break;
	default:
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
		break;

	}
}

void allInit(void){
	gpio_reset_pin(LED0);
	gpio_reset_pin(LED1);
	gpio_reset_pin(LED2);
	gpio_reset_pin(LED3);

	gpio_reset_pin(SW_R);
	gpio_reset_pin(SW_C);
	gpio_reset_pin(SW_L);


	gpio_set_direction(LED0, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

	gpio_set_direction(SW_R, GPIO_MODE_INPUT);
	gpio_set_direction(SW_C, GPIO_MODE_INPUT);
	gpio_set_direction(SW_L, GPIO_MODE_INPUT);

	ledc_timer_config_t ledc_timer = {
		.speed_mode			= LEDC_LOW_SPEED_MODE,
		.duty_resolution	= 10,
		.timer_num			= LEDC_TIMER_0,
		.freq_hz			= 440,
		.clk_cfg			= LEDC_AUTO_CLK
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		.speed_mode			= LEDC_LOW_SPEED_MODE,
		.channel			= LEDC_CHANNEL_0,
		.timer_sel			= LEDC_TIMER_0,
		.intr_type			= LEDC_INTR_DISABLE,
		.gpio_num			= BUZZER,
		.duty				= 0,
		.hpoint				= 0
	};
	ledc_channel_config(&ledc_channel);
}

void app_main(void)
{
	allInit();

	g_mode = 1;
	ledSet(g_mode);

    while (1) {
        while(gpio_get_level(SW_L) & gpio_get_level(SW_C) & gpio_get_level(SW_R)){
        	vTaskDelay(10 / portTICK_PERIOD_MS);
        	continue;
        }
        if(gpio_get_level(SW_R) == 0){
        	g_mode++;
        	if(g_mode > 15){
        		g_mode = 1;
        	}else{
        		ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, INC_FREQ);
        		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
        		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        		vTaskDelay(30 / portTICK_PERIOD_MS);
        		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
        		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        	}
        	ledSet(g_mode);
        }
		if(gpio_get_level(SW_L) == 0){
			g_mode--;
			if(g_mode > 1){
				g_mode = 1;
			}else{
				ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, DEC_FREQ);
				ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
				ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
				vTaskDelay(30 / portTICK_PERIOD_MS);
				ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
				ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			}
			ledSet(g_mode);
		}
		
        if(gpio_get_level(SW_C) == 0){
        	ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, INC_FREQ);
        	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
        	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        	vTaskDelay(80 / portTICK_PERIOD_MS);
        	ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, DEC_FREQ);
        	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
        	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        	vTaskDelay(80 / portTICK_PERIOD_MS);
        	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1024);
        	ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        	vTaskDelay(300 / portTICK_PERIOD_MS);
        	modeExec(g_mode);
        }
        while(!(gpio_get_level(SW_L) & gpio_get_level(SW_R))){
        	vTaskDelay(10 / portTICK_PERIOD_MS);
        	continue;
        }
        vTaskDelay(30/portTICK_PERIOD_MS);
    }
}
