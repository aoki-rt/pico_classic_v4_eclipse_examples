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

#include "device.h"

DEVICE g_device;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//interrupt call
static bool IRAM_ATTR onTimer0Cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	runInterruptControl();
	portEXIT_CRITICAL(&timerMux);

	return 0;
}

static bool IRAM_ATTR onTimer1Cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	sensorInterruptControl();
	portEXIT_CRITICAL(&timerMux);

	return 0;
}


static bool IRAM_ATTR isrR(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	runIntteruptRight();
	portEXIT_CRITICAL(&timerMux);

	return 0;
}

static bool IRAM_ATTR isrL(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
	portENTER_CRITICAL(&timerMux);
	runIntteruptLeft();
	portEXIT_CRITICAL(&timerMux);

	return 0;
}

//esp-idf to Arduino function
void delay(int timer)
{
	vTaskDelay((TickType_t)(timer/portTICK_PERIOD_MS));
}

void DEVICE::ledcSetup(ledc_channel_t ledc_num,int hz,int resolution)
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

void DEVICE::ledcAttachPin(int io_num,ledc_channel_t ledc_num)
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

void DEVICE::ledcWrite (ledc_channel_t ledc_num,int duty){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_num, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_num);
}

void DEVICE::ledcWritetone(ledc_channel_t ledc_num,int hz){
	ledc_timer_t timer_num=(ledc_timer_t)(ledc_num/LEDC_CHANNEL_2);

    if(!hz){
    	ledcWrite(ledc_num,0);
    }

	ledc_set_freq(LEDC_LOW_SPEED_MODE,timer_num,hz);
	ledcWrite(ledc_num, 0x1FF);
}



void DEVICE::timerBegin(gptimer_handle_t *timer,int resolution_hz ,gptimer_count_direction_t counter){
    gptimer_config_t timer_config = {};
    timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    timer_config.direction = counter;
	timer_config.resolution_hz = resolution_hz;
	gptimer_new_timer(&timer_config, timer);
}

void DEVICE::timerAttachInterrupt(gptimer_handle_t timer, bool (*fn)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)){
	gptimer_event_callbacks_t cbs = {};
	cbs.on_alarm = fn;
	gptimer_register_event_callbacks(timer, &cbs, NULL);
}

void DEVICE::timerAlarmWrite(gptimer_handle_t timer, int period, bool reload){
	gptimer_alarm_config_t alarm_config = {};
	alarm_config.reload_count = 0;
	alarm_config.alarm_count = period;
	alarm_config.flags.auto_reload_on_alarm = reload;
	gptimer_set_alarm_action(timer, &alarm_config);
}

void DEVICE::timerAlarmEnable(gptimer_handle_t timer){
	gptimer_enable(timer);
	gptimer_start(timer);
}

void DEVICE::pwmtimerBegin(mcpwm_timer_handle_t * pwmtimer, int resolution_hz,mcpwm_oper_handle_t *pwmoprator, mcpwm_cmpr_handle_t * comparator){


//pwm timer
	mcpwm_timer_config_t timer_config = {};
	timer_config.group_id = 1;
	timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
	timer_config.resolution_hz = resolution_hz;
	timer_config.period_ticks = resolution_hz/40;
	timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
	mcpwm_new_timer(&timer_config,pwmtimer);

//operator
	mcpwm_operator_config_t operator_config = {};
	operator_config.group_id = 1;
	mcpwm_new_operator(&operator_config, pwmoprator);

//connect timer and operatot
	mcpwm_operator_connect_timer(*pwmoprator, *pwmtimer);

//comparator
	mcpwm_comparator_config_t comparator_config = {};
	comparator_config.flags.update_cmp_on_tez = true;
	mcpwm_new_comparator(*pwmoprator, &comparator_config, comparator);

}

void DEVICE::pwmtimerAttachInterrupt(mcpwm_timer_handle_t timer, bool (*fn)(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)){
	mcpwm_timer_event_callbacks_t tim_cbs = {};
	tim_cbs.on_empty = fn;
	mcpwm_timer_register_event_callbacks(timer, &tim_cbs, NULL);
}


void DEVICE::pwmtimerAttachPin(mcpwm_oper_handle_t pwmoprator , mcpwm_cmpr_handle_t  comparator,int io_num){
	mcpwm_gen_handle_t generator = NULL;

	mcpwm_generator_config_t generator_config = {};
	generator_config.gen_gpio_num = io_num;
	mcpwm_new_generator(pwmoprator, &generator_config, &generator);

	// go high on counter empty
	mcpwm_gen_timer_event_action_t timer_event = {};
	timer_event.direction = MCPWM_TIMER_DIRECTION_UP;
	timer_event.event = MCPWM_TIMER_EVENT_EMPTY;
	timer_event.action = MCPWM_GEN_ACTION_HIGH;
	mcpwm_generator_set_action_on_timer_event(generator,timer_event);

	// go low on compare threshold
	mcpwm_gen_compare_event_action_t compare_event = {};
	compare_event.direction = MCPWM_TIMER_DIRECTION_UP;
	compare_event.comparator = comparator;
	compare_event.action = MCPWM_GEN_ACTION_LOW;

	mcpwm_generator_set_action_on_compare_event(generator,compare_event);

}

void DEVICE::adc1Begin(adc_oneshot_unit_handle_t *adc1){
	adc_oneshot_unit_init_cfg_t init_config = {};
	init_config.unit_id = ADC_UNIT_1;
	adc_oneshot_new_unit(&init_config, adc1);
}

void DEVICE::adc1AttachPin(adc_oneshot_unit_handle_t adc1,int io_num){
	adc_oneshot_chan_cfg_t config = {};
	config.bitwidth = ADC_BITWIDTH_DEFAULT;
	config.atten = ADC_ATTEN;
	adc_oneshot_config_channel(adc1, (adc_channel_t)io_num, &config);
}

void DEVICE::adc1Cali(int io_num){
	adc_cali_curve_fitting_config_t cali_config = {};
	cali_config.unit_id	=	ADC_UNIT_1;
	cali_config.chan 		=	(adc_channel_t)io_num;
	cali_config.atten		=	ADC_ATTEN;
	cali_config.bitwidth	=	ADC_BITWIDTH_DEFAULT;
	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);
}


void DEVICE::allInit(void)
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
    gpio_reset_pin((gpio_num_t)SW_R);
    gpio_set_direction((gpio_num_t)SW_R, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)SW_L, GPIO_MODE_INPUT);

	gpio_reset_pin((gpio_num_t)SLED_F);
	gpio_reset_pin((gpio_num_t)SLED_S);
	gpio_set_direction((gpio_num_t)SLED_F, GPIO_MODE_OUTPUT);
	gpio_set_direction((gpio_num_t)SLED_S, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)SLED_F, 0);
    gpio_set_level((gpio_num_t)SLED_S, 0);


    ledcSetup(LEDC_CHANNEL_0, 440, 10);
    ledcAttachPin(BUZZER, LEDC_CHANNEL_0);
    ledcWrite(LEDC_CHANNEL_0, 0);

    gpio_reset_pin((gpio_num_t)MOTOR_EN);
    gpio_set_direction((gpio_num_t)MOTOR_EN, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)MOTOR_EN, 0);

    gpio_reset_pin((gpio_num_t)CW_R);
    gpio_reset_pin((gpio_num_t)CW_L);
    gpio_set_direction((gpio_num_t)CW_R, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)CW_L, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)CW_R, 1);
    gpio_set_level((gpio_num_t)CW_L, 0);

	timerBegin(&timer0,1000000,GPTIMER_COUNT_UP);
	timerAttachInterrupt(timer0,&onTimer0Cb);
    timerAlarmWrite(timer0,1000,true);
    timerAlarmEnable(timer0);

	timerBegin(&timer1,1000000,GPTIMER_COUNT_UP);
	timerAttachInterrupt(timer1,&onTimer1Cb);
    timerAlarmWrite(timer1,500,true);
    timerAlarmEnable(timer1);

    adc1Begin(&adc1_handle);
    adc1AttachPin(adc1_handle,AD0);
    adc1AttachPin(adc1_handle,AD1);
    adc1AttachPin(adc1_handle,AD2);
    adc1AttachPin(adc1_handle,AD3);
    adc1AttachPin(adc1_handle,AD4);
    adc1Cali(AD0);

	pwmtimerBegin(&pwmtimer_r,2000000,&pwmoprator_r,&comparator_r);
	mcpwm_comparator_set_compare_value(comparator_r, 0);
	pwmtimerAttachInterrupt(pwmtimer_r,isrR);
	pwmtimerAttachPin(pwmoprator_r,comparator_r,PWM_R);

	pwmtimerBegin(&pwmtimer_l,2000000,&pwmoprator_l,&comparator_l);
	mcpwm_comparator_set_compare_value(comparator_l, 0);
	pwmtimerAttachInterrupt(pwmtimer_l,isrL);
	pwmtimerAttachPin(pwmoprator_l,comparator_l,PWM_L);

    mcpwm_timer_enable(pwmtimer_r);
    mcpwm_timer_enable(pwmtimer_l);

    motorEnable();
   	delay(200);
   	motorDisable();

    buzzerEnable(INC_FREQ);
    delay(80);
    buzzerDisable();

}



//LED
void DEVICE::LEDSet(int led_data)
{
	gpio_set_level((gpio_num_t)LED0,led_data&0x01);
	gpio_set_level((gpio_num_t)LED1,(led_data&0x02)>>1);
	gpio_set_level((gpio_num_t)LED2,(led_data&0x04)>>2);
	gpio_set_level((gpio_num_t)LED3,(led_data&0x08)>>3);
}

//buzzer
void DEVICE::buzzerEnable(short f) {
	ledcWritetone(BUZZER_CH, f);
}

void DEVICE::buzzerDisable(void)
{
    ledcWrite(BUZZER_CH, 0);  //duty 100% Buzzer OFF
}

//motor
void DEVICE::motorEnable(void)
{
	gpio_set_level((gpio_num_t)MOTOR_EN,1);//Power On
}

void DEVICE::motorDisable(void)
{
	gpio_set_level((gpio_num_t)MOTOR_EN,0);//Power Off
}

void DEVICE::motorMoveDir(t_CW_CCW left_CW, t_CW_CCW right_CW)
{
	if(right_CW == MOT_FORWARD)
	{
		gpio_set_level((gpio_num_t)CW_R,1);
	} else{
		gpio_set_level((gpio_num_t)CW_R,0);
	}

	if(left_CW == MOT_FORWARD)
	{
		gpio_set_level((gpio_num_t)CW_L,0);
	} else{
		gpio_set_level((gpio_num_t)CW_L,1);
	}

}

//switch
unsigned char DEVICE::switchGet(void)
{
	unsigned char ret = 0;
	  if (gpio_get_level((gpio_num_t)SW_R) == 0) {
	    do {
	      delay(20);
	    } while (gpio_get_level((gpio_num_t)SW_R) == 0);
	    ret |= SW_RM;
	  }
	  if (gpio_get_level((gpio_num_t)SW_L) == 0) {
	    do {
	      delay(20);
	    } while (gpio_get_level((gpio_num_t)SW_L) == 0);
	    ret |= SW_LM;
	  }
	  return ret;
}


void DEVICE::pwmHzSet(int step_hz_l,int step_hz_r){
	mcpwm_timer_set_period(pwmtimer_r,2000000 / step_hz_r);
	mcpwm_timer_set_period(pwmtimer_l,2000000 / step_hz_l);
	mcpwm_comparator_set_compare_value(comparator_r, 2000000 / step_hz_r/2);
	mcpwm_comparator_set_compare_value(comparator_l, 2000000 / step_hz_l/2);
}

void DEVICE::pwmHzSetRight(int step_hz_r){
	mcpwm_timer_set_period(pwmtimer_r,2000000 / step_hz_r);
	mcpwm_comparator_set_compare_value(comparator_r, 2000000 / step_hz_r/2);
}

void DEVICE::pwmHzSetLeft(int step_hz_l){
	mcpwm_timer_set_period(pwmtimer_l,2000000 / step_hz_l);
	mcpwm_comparator_set_compare_value(comparator_l, 2000000 / step_hz_l/2);
}

void DEVICE::pwmtimerStart(void)
{
	portENTER_CRITICAL(&timerMux);
	mcpwm_timer_start_stop(pwmtimer_r, MCPWM_TIMER_START_NO_STOP);
	mcpwm_timer_start_stop(pwmtimer_l, MCPWM_TIMER_START_NO_STOP);
	portEXIT_CRITICAL(&timerMux);
}

void DEVICE::pwmtimerStop(void)
{
	portENTER_CRITICAL(&timerMux);
	mcpwm_timer_start_stop(pwmtimer_r,MCPWM_TIMER_STOP_FULL);
	mcpwm_timer_start_stop(pwmtimer_l,MCPWM_TIMER_STOP_FULL);
	portEXIT_CRITICAL(&timerMux);
}

void DEVICE::sensorGetF(volatile short *fl_value, volatile short *fr_value){
	volatile int temp_fr, temp_fl,temp;
	adc_oneshot_read(adc1_handle, AD4, (int *)&temp_fr);
	adc_oneshot_read(adc1_handle, AD1, (int *)&temp_fl);
	gpio_set_level((gpio_num_t)SLED_F,1);	//LED on
	for (int i = 0; i < 10; i++);
    adc_oneshot_read(adc1_handle, AD4, (int *)&temp);
    *fr_value = temp - temp_fr;
    adc_oneshot_read(adc1_handle, AD1, (int *)&temp);
    *fl_value = temp - temp_fl;
    gpio_set_level((gpio_num_t)SLED_F,0);	//LED off

}
void DEVICE::sensorGetS(volatile short *l_value, volatile short *r_value){
	volatile int temp_r, temp_l,temp;

	adc_oneshot_read(adc1_handle, AD3, (int *)&temp_r);
	adc_oneshot_read(adc1_handle, AD2, (int *)&temp_l);

	gpio_set_level((gpio_num_t)SLED_S,1);	//LED on
	for (int i = 0; i < 10; i++);
	adc_oneshot_read(adc1_handle, AD3, (int *)&temp);
	*r_value = temp - temp_r;
	adc_oneshot_read(adc1_handle, AD2, (int *)&temp);
	*l_value = temp - temp_l;
	gpio_set_level((gpio_num_t)SLED_S,0);	//LED off

}

short DEVICE::voltageGet(void)
{
	int temp,v_temp;
	adc_oneshot_read(adc1_handle, AD0, &temp);
	adc_cali_raw_to_voltage(adc1_cali_handle,temp, &v_temp);
	return (short)v_temp;
}


