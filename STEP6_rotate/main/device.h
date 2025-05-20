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


#ifndef MAIN_DEVICE_H_
#define MAIN_DEVICE_H_


#define SW_LM 1
#define SW_CM 2
#define SW_RM 4

#define LED0 13
#define LED1 14
#define LED2 47
#define LED3 48

#define SW_L 16
#define SW_C 18
#define SW_R 15


#define MOTOR_EN 17

#define SPI_CLK 39
#define SPI_MOSI 42
#define SPI_MISO 41
#define SPI_CS_L 40
#define SPI_CS_R 3
#define SPI_CS_J 46


#define BUZZER_CH LEDC_CHANNEL_0


typedef enum {
	front,
	right,
	rear,
	left,
} t_direction;


void allInit(void);
void delay(int timer);
void ledSet(int led_data);
void motorEnable(void);
void motorDisable(void);
void spiRead32(unsigned char cmd_data, int *read_data, char cs);
void spiWrite32(unsigned char cmd_data, int write_data, char cs);
unsigned char switchGet(void);


#endif /* MAIN_DEVICE_H_ */
