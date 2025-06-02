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
#include "device.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "parameter.h"
//#include "flash.h"

char buf_tx[100];


extern void controlInterrupt(void);
extern void sensorInterrupt(void);

    gptimer_handle_t timer0 = NULL;
    gptimer_handle_t timer1 = NULL;

    adc_oneshot_unit_handle_t adc1_handle = NULL;
    adc_cali_handle_t adc1_cali_handle = NULL;
	
	spi_transaction_t t;
	spi_device_handle_t spi; 
	
	portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;	

/****************************************************************************************************/
//interrupt call
/****************************************************************************************************/
static bool IRAM_ATTR onTimer0Cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	controlInterrupt();
	portEXIT_CRITICAL(&timerMux);
	return 0;
}

static bool IRAM_ATTR onTimer1Cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	sensorInterrupt();
	portEXIT_CRITICAL(&timerMux);
	return 0;
}


/*****************************************************************************************************/
//device.c内部で使用する関数
/*****************************************************************************************************/
void adc1Begin(adc_oneshot_unit_handle_t *adc1){
	adc_oneshot_unit_init_cfg_t init_config = {};
	init_config.unit_id = ADC_UNIT_1;
	adc_oneshot_new_unit(&init_config, adc1);
}

void adc1AttachPin(adc_oneshot_unit_handle_t adc1,int io_num){
	adc_oneshot_chan_cfg_t config = {};
	config.bitwidth = ADC_BITWIDTH_DEFAULT;
	config.atten = ADC_ATTEN;
	adc_oneshot_config_channel(adc1, (adc_channel_t)io_num, &config);
}

void adc1Cali(int io_num){
	adc_cali_curve_fitting_config_t cali_config = {};
	cali_config.unit_id	=	ADC_UNIT_1;
	cali_config.chan 		=	(adc_channel_t)io_num;
	cali_config.atten		=	ADC_ATTEN;
	cali_config.bitwidth	=	ADC_BITWIDTH_DEFAULT;
	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);
}


void ledcSetup(ledc_channel_t ledc_num,int hz,int resolution)
{
	ledc_timer_t timer_num=(ledc_timer_t)(ledc_num/LEDC_CHANNEL_2);

	ledc_timer_config_t ledc_timer={};
	ledc_timer.speed_mode		= LEDC_LOW_SPEED_MODE;
	ledc_timer.duty_resolution	= (ledc_timer_bit_t)10;
	ledc_timer.timer_num		= timer_num;
	ledc_timer.freq_hz			= (uint32_t)hz;
	ledc_timer.clk_cfg			= LEDC_AUTO_CLK;

	ledc_timer_config(&ledc_timer);
}

void ledcAttachPin(int io_num,ledc_channel_t ledc_num)
{
	ledc_timer_t timer_num=(ledc_timer_t)(ledc_num/LEDC_CHANNEL_2);
	ledc_channel_config_t ledc_channel={};
	ledc_channel.speed_mode			= LEDC_LOW_SPEED_MODE;
	ledc_channel.channel			= ledc_num;
	ledc_channel.timer_sel			= timer_num;
	ledc_channel.intr_type			= LEDC_INTR_DISABLE;
	ledc_channel.gpio_num			= io_num;
	ledc_channel.duty				= 0;
	ledc_channel.hpoint				= 0;

	ledc_channel_config(&ledc_channel);
}


void ledcWrite (ledc_channel_t ledc_num,int duty){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_num, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_num);
}

void ledcWritetone(ledc_channel_t ledc_num,int hz){
	ledc_timer_t timer_num=(ledc_timer_t)(ledc_num/LEDC_CHANNEL_2);

    if(!hz){
    	ledcWrite(ledc_num,0);
    }
	ledc_set_freq(LEDC_LOW_SPEED_MODE,timer_num,hz);
	ledcWrite(ledc_num, 0x1FF);
}


void timerBegin(gptimer_handle_t *timer,int resolution_hz ,gptimer_count_direction_t counter){
    gptimer_config_t timer_config = {};
    timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    timer_config.direction = counter;
	timer_config.resolution_hz = resolution_hz;
	gptimer_new_timer(&timer_config, timer);
}

void timerAttachInterrupt(gptimer_handle_t timer, bool (*fn)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)){
	gptimer_event_callbacks_t cbs = {};
	cbs.on_alarm = fn;
	gptimer_register_event_callbacks(timer, &cbs, NULL);
}

void timerAlarmWrite(gptimer_handle_t timer, int period, bool reload){
	gptimer_alarm_config_t alarm_config = {};
	alarm_config.reload_count = 0;
	alarm_config.alarm_count = period;
	alarm_config.flags.auto_reload_on_alarm = reload;
	gptimer_set_alarm_action(timer, &alarm_config);
}

void timerAlarmEnable(gptimer_handle_t timer){
	gptimer_enable(timer);
	gptimer_start(timer);
}



void spiInit(void){
	spi_bus_config_t buscfg = {
		.miso_io_num = SPI_MISO,
	    .mosi_io_num = SPI_MOSI,
	    .sclk_io_num = SPI_CLK,
	    .quadwp_io_num = -1,
	    .quadhd_io_num = -1,
	    .max_transfer_sz =  8
	};
	spi_device_interface_config_t devcfg = {
	    .clock_speed_hz = 5000000,
	    .mode = 3,
	    .spics_io_num = -1,
	    .queue_size = 7, 
	};	
	//Initialize the SPI bus
	spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	spi_bus_add_device(SPI2_HOST, &devcfg, &spi);	
} 

void tmc5240PortInit(void){
	gpio_reset_pin(SPI_CS_L);
	gpio_reset_pin(SPI_CS_R);
	gpio_reset_pin(SPI_CS_J);
	gpio_set_direction(SPI_CS_L, GPIO_MODE_OUTPUT);
	gpio_set_direction(SPI_CS_R, GPIO_MODE_OUTPUT);
	gpio_set_direction(SPI_CS_J, GPIO_MODE_OUTPUT);

	gpio_set_level(SPI_CS_L,1);
	gpio_set_level(SPI_CS_R,1);  
	gpio_set_level(SPI_CS_J,1);
}


/***************************************************************************************************/

void allInit(void)
{
    gpio_reset_pin((gpio_num_t)LED0);
    gpio_reset_pin((gpio_num_t)LED1);
    gpio_reset_pin((gpio_num_t)LED2);
    gpio_reset_pin((gpio_num_t)LED3);
    gpio_set_direction((gpio_num_t)LED0, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)LED2, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)LED3, GPIO_MODE_OUTPUT);

    gpio_reset_pin((gpio_num_t)BLED0);
    gpio_reset_pin((gpio_num_t)BLED1);
    gpio_set_direction((gpio_num_t)BLED0, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)BLED1, GPIO_MODE_OUTPUT);

	gpio_reset_pin((gpio_num_t)SW_L);
	gpio_reset_pin((gpio_num_t)SW_C);	
	gpio_reset_pin((gpio_num_t)SW_R);
	gpio_set_direction((gpio_num_t)SW_R, GPIO_MODE_INPUT);
	gpio_set_direction((gpio_num_t)SW_C, GPIO_MODE_INPUT);
	gpio_set_direction((gpio_num_t)SW_L, GPIO_MODE_INPUT);

    ledcSetup(LEDC_CHANNEL_0, 440, 10);
    ledcAttachPin(BUZZER, LEDC_CHANNEL_0);
    ledcWrite(LEDC_CHANNEL_0, 0);

    gpio_reset_pin((gpio_num_t)MOTOR_EN);
    gpio_set_direction((gpio_num_t)MOTOR_EN, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)MOTOR_EN, 0);

	gpio_reset_pin(SLED_FR);
	gpio_reset_pin(SLED_FL);
	gpio_reset_pin(SLED_R);
	gpio_reset_pin(SLED_L);
	gpio_set_direction(SLED_FR, GPIO_MODE_OUTPUT);
	gpio_set_direction(SLED_FL, GPIO_MODE_OUTPUT);
	gpio_set_direction(SLED_R, GPIO_MODE_OUTPUT);
	gpio_set_direction(SLED_L, GPIO_MODE_OUTPUT);
	gpio_set_level((gpio_num_t)SLED_FR, 0);
	gpio_set_level((gpio_num_t)SLED_FL, 0);
	gpio_set_level((gpio_num_t)SLED_R, 0);
	gpio_set_level((gpio_num_t)SLED_L, 0);

	timerBegin(&timer0,1000000,GPTIMER_COUNT_UP);
	timerAttachInterrupt(timer0,&onTimer0Cb);
    timerAlarmWrite(timer0,1000,true);
    timerAlarmEnable(timer0);

	timerBegin(&timer1,1000000,GPTIMER_COUNT_UP);
	timerAttachInterrupt(timer1,&onTimer1Cb);
    timerAlarmWrite(timer1,250,true);
    timerAlarmEnable(timer1);

    adc1Begin(&adc1_handle);
    adc1AttachPin(adc1_handle,AD0);
    adc1AttachPin(adc1_handle,AD1);
    adc1AttachPin(adc1_handle,AD2);
    adc1AttachPin(adc1_handle,AD3);
    adc1AttachPin(adc1_handle,AD4);
    adc1Cali(AD0);



	motorEnable();
	delay(1);
	tmc5240PortInit();
	spiInit();
	motorDisable();

    buzzerEnable(INC_FREQ);
    delay(80);
    buzzerDisable();

}


void delay(int timer)
{
	vTaskDelay((TickType_t)(timer/portTICK_PERIOD_MS));
}

//LED
void ledSet(int led_data)
{
	gpio_set_level((gpio_num_t)LED0,led_data&0x01);
	gpio_set_level((gpio_num_t)LED1,(led_data&0x02)>>1);
	gpio_set_level((gpio_num_t)LED2,(led_data&0x04)>>2);
	gpio_set_level((gpio_num_t)LED3,(led_data&0x08)>>3);
}

void bledSet(char data)
{
	if (data & 0x01) {
		gpio_set_level((gpio_num_t)BLED0, 1);
	} else {
		gpio_set_level((gpio_num_t)BLED0, 0);
	}
	if (data & 0x02) {
		gpio_set_level((gpio_num_t)BLED1, 1);
	} else {
		gpio_set_level((gpio_num_t)BLED1, 0);
	}
}

//buzzer
void buzzerEnable(short f) {
	ledcWritetone(BUZZER_CH, f);
}

void buzzerDisable(void)
{
    ledcWrite(BUZZER_CH, 0);  //duty 100% Buzzer OFF
}

//switch
unsigned char switchGet(void)
{
	unsigned char ret = 0;
	if (gpio_get_level((gpio_num_t)SW_R) == 0) {
		do {
			delay(20);
		} while (gpio_get_level((gpio_num_t)SW_R) == 0);
		ret |= SW_RM;
	}
	if (gpio_get_level((gpio_num_t)SW_C) == 0) {
		do {
			delay(20);
		} while (gpio_get_level((gpio_num_t)SW_C) == 0);
		ret |= SW_CM;
	}
	if (gpio_get_level((gpio_num_t)SW_L) == 0) {
		do {
			delay(20);
		} while (gpio_get_level((gpio_num_t)SW_L) == 0);
		ret |= SW_LM;
	}
	return ret;
}

/*************************************************************************************/
//motor
/*************************************************************************************/
void motorEnable(void)
{
	gpio_set_level((gpio_num_t)MOTOR_EN,0);//Power On
}

void motorDisable(void)
{
	gpio_set_level((gpio_num_t)MOTOR_EN,1);//Power Off
}


/*****************************************************************************/
//Timer Start or Stop
/*****************************************************************************/
	//Speed Control Timer
void controlInterruptStart(void){gptimer_start(timer0);}
void controlInterruptStop(void){gptimer_stop(timer0);}

	//Sensor Control Timer
void sensorInterruptStart(void){gptimer_start(timer1);}
void sensorInterruptStop(void){gptimer_stop(timer1);}


//*****************************************************************************/
//Sensor A/D
/******************************************************************************/
unsigned short sensorGetFR(void){
	int temp_fr;
	gpio_set_level((gpio_num_t)SLED_FR,1);	//LED on
	for (int i = 0; i < 10; i++){
		asm("nop \n");
	}
    adc_oneshot_read(adc1_handle, AD1, &temp_fr);
    gpio_set_level((gpio_num_t)SLED_FR,0);	//LED off

	return temp_fr;
}

unsigned short sensorGetFL(void){
	int temp_fl;
	gpio_set_level((gpio_num_t)SLED_FL,1);	//LED on
	for (int i = 0; i < 10; i++){
			asm("nop \n");
		}
    adc_oneshot_read(adc1_handle, AD2, &temp_fl);
    gpio_set_level((gpio_num_t)SLED_FL,0);	//LED off

	return temp_fl;
}

unsigned short sensorGetR(void){
	int temp_r;
	gpio_set_level((gpio_num_t)SLED_R,1);	//LED on
	for (int i = 0; i < 10; i++){
		asm("nop \n");
	}
	adc_oneshot_read(adc1_handle, AD3, &temp_r);
	gpio_set_level((gpio_num_t)SLED_R,0);	//LED off

	return temp_r;
}

unsigned short sensorGetL(void){
	int temp_l;
	gpio_set_level((gpio_num_t)SLED_L,1);	//LED on
	for (int i = 0; i < 10; i++){
		asm("nop \n");
	}
	adc_oneshot_read(adc1_handle, AD4, &temp_l);
	gpio_set_level((gpio_num_t)SLED_L,0);	//LED off
	return temp_l;

}

short batteryVoltGet(void)
{
	int temp,v_temp;
	adc_oneshot_read(adc1_handle, AD0, &temp);
	adc_cali_raw_to_voltage(adc1_cali_handle,temp, &v_temp);
	return (short)v_temp/ 10.0 * (10.0 + 51.0);
}

//*****************************************************************************/
//SPI
/******************************************************************************/
void spiRead32(unsigned char cmd_data, int *read_data, char cs){
// When using SPI_TRANS_CS_KEEP_ACTIVE, bus must be locked/acquired
	spi_device_acquire_bus(spi, portMAX_DELAY);

	//レジスタのアドレス
	memset(&t, 0, sizeof(t));       //Zero out the transaction
	t.length = 8;                   //8 bits
	t.tx_buffer = &cmd_data;
	t.flags = SPI_TRANS_CS_KEEP_ACTIVE;

	gpio_set_level((gpio_num_t)cs,0);
	spi_device_polling_transmit(spi, &t); 

	//データの書き込み
	memset(&t, 0, sizeof(t));
	t.length = 8 * 4;
	t.flags = SPI_TRANS_USE_RXDATA;
	spi_device_polling_transmit(spi, &t);
	gpio_set_level((gpio_num_t)cs,1);
	spi_device_release_bus(spi);
	
		
	*read_data = 	(((uint32_t)t.rx_data[0])<<24)+
					(((uint32_t)t.rx_data[1])<<16)+
					(((uint32_t)t.rx_data[2])<<8)+
					(uint32_t)t.rx_data[3];
}

void spiWrite32(unsigned char cmd_data, int write_data, char cs){
// When using SPI_TRANS_CS_KEEP_ACTIVE, bus must be locked/acquired
	spi_device_acquire_bus(spi, portMAX_DELAY);

	//レジスタのアドレス
	memset(&t, 0, sizeof(t));       //Zero out the transaction
	t.length = 8;                   //8 bits
	t.tx_buffer = &cmd_data;
	t.flags = SPI_TRANS_CS_KEEP_ACTIVE;

	gpio_set_level((gpio_num_t)cs,0);
	spi_device_polling_transmit(spi, &t); 
		
	//データの書き込み
	memset(&t, 0, sizeof(t));
	t.length = 8 * 4;		//32bit
	t.flags = SPI_TRANS_USE_TXDATA;
	t.tx_data[0]=(write_data>>24)&0xff;
	t.tx_data[1]=(write_data>>16)&0xff;
	t.tx_data[2]=(write_data>>8)&0xff;
	t.tx_data[3]=(write_data)&0xff;
	
	spi_device_polling_transmit(spi, &t);
	
	gpio_set_level((gpio_num_t)cs,1);
	spi_device_release_bus(spi);

}

void usbPrintf(const char *format, ...){
	va_list arg;
	int length;

	va_start(arg, format);
	length = vsprintf (buf_tx, format, arg);
	va_end(arg);	
	
	printf(buf_tx);
	
}

