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

extern "C"{
	#include "device.h"
}

#include "run.h"
#include "sensor.h"
#include "parameter.h"
#include "TMC5240.h"


RUN g_run;

RUN::RUN(){
	speed = 0.0f;
	accel = 0.0f;
	speed_target_r = 0;
	speed_target_l = 0;
	upper_speed_limit = 0;
	lower_speed_limit = 0;
	step_lr_len=0;
	step_lr=0;
	con_wall.kp = CON_WALL_KP;
}

extern "C" void controlInterrupt(void){
	g_run.interrupt();
}

void RUN::interrupt(void)
{

	speed = speed + accel;

	if (speed > upper_speed_limit) {
	  speed = upper_speed_limit;
	}
	if (speed < lower_speed_limit) {
	  speed = lower_speed_limit;
	}

	if(con_wall.enable == true){
		con_wall.p_error = con_wall.error;

		if ((g_sensor.sen_r.is_control == true) && (g_sensor.sen_l.is_control == true)) {
			con_wall.error = g_sensor.sen_r.error - g_sensor.sen_l.error;
		} else {
			con_wall.error = 2.0 * (g_sensor.sen_r.error - g_sensor.sen_l.error);
		}

		con_wall.diff = con_wall.error - con_wall.p_error;
		con_wall.sum += con_wall.error;
		if (con_wall.sum > con_wall.sum_max) {
			con_wall.sum = con_wall.sum_max;
		} else if (con_wall.sum < (-con_wall.sum_max)) {
			con_wall.sum = -con_wall.sum_max;
		}
		con_wall.control = 0.001 * speed * con_wall.kp * con_wall.error;
		speed_target_r = speed + con_wall.control;
		speed_target_l = speed - con_wall.control;
	} else {
		speed_target_r = speed;
		speed_target_l = speed;
  	}
	if (speed_target_r < MIN_SPEED) speed_target_r = MIN_SPEED;
	if (speed_target_l < MIN_SPEED) speed_target_l = MIN_SPEED;

}


void RUN::dirSet(t_CW_CCW dir_left, t_CW_CCW dir_right)
{
  g_tmc5240.write(TMC5240_RAMPMODE, dir_left, dir_right);
}

void RUN::counterClear(void) { g_tmc5240.write(TMC5240_XACTUAL, 0, 0); }

void RUN::speedSet(double l_speed, double r_speed)
{
  g_tmc5240.write(
    TMC5240_VMAX, (unsigned int)(l_speed / (PULSE * 0.787)),
    (unsigned int)(r_speed / (PULSE * 0.787)));
}

void RUN::stay(double l_speed) {
  controlInterruptStop();
  upper_speed_limit = lower_speed_limit = speed = l_speed;
  accel = 0.0;
  counterClear();
  speedSet(l_speed, l_speed);
  controlInterruptStart();
}

void RUN::stepGet(void)
{
  step_lr = g_tmc5240.readXactual();
  step_lr_len = (int)((float)step_lr / 2.0 * PULSE);
}

void RUN::stop(void) {
	g_tmc5240.write(TMC5240_VMAX, 0, 0);
	delay(300); 
}


void RUN::straight(int len, int init_speed, int max_sp, int finish_speed)
{
	int obj_step;

  controlInterruptStop();
	upper_speed_limit = (double)max_sp;
	accel = SEARCH_ACCEL;

	if (init_speed < MIN_SPEED) {
		speed = (double)MIN_SPEED;
	} else {
		speed = (double)init_speed;
	}
	if (finish_speed < MIN_SPEED) {
		finish_speed = MIN_SPEED;
	}
	lower_speed_limit = (double)finish_speed;

	con_wall.enable = true;
	speedSet(speed, speed);
	dirSet(MOT_FORWARD, MOT_FORWARD);
	obj_step = (int)((float)len * 2.0 / PULSE);
	controlInterruptStart();

	while (1){
		stepGet();
		speedSet(speed_target_l, speed_target_r);		
		if((int)(len - step_lr_len) <
			 (int)(((speed * speed) - (lower_speed_limit * lower_speed_limit)) / (2.0 * 1000.0 * accel))) {
				break;
		}
	
	}
	accel = -1.0 * accel;

	while (1){
		stepGet();
		speedSet(speed_target_l, speed_target_r);
		if(step_lr  < obj_step) {
			break;
		}
	}

	if (finish_speed == MIN_SPEED) {
		stop();
	}else{
		stay(finish_speed);
	}
}


void RUN::accelerate(int len, int finish_speed)
{
	int obj_step;

	controlInterruptStop();
	accel = SEARCH_ACCEL;
	speed = lower_speed_limit = (double)MIN_SPEED;
	upper_speed_limit = (double)finish_speed;
	con_wall.enable = true;	
	dirSet(MOT_FORWARD,MOT_FORWARD);
	speedSet(speed,speed);
	counterClear();	
	obj_step = (int)((float)len * 2.0 /PULSE);
	controlInterruptStart();

	while (1){
		stepGet();
   		speedSet(speed_target_l, speed_target_r);
		if(step_lr > obj_step){
			break;
		}
	}
	stay(finish_speed);
}

void RUN::oneStep(int len, int init_speed)
{
	int obj_step;
	
	controlInterruptStop();	
	accel = 0.0;	
	speed = lower_speed_limit = upper_speed_limit = (float)init_speed;
	con_wall.enable = true;
	dirSet(MOT_FORWARD,MOT_FORWARD);
	speedSet(speed,speed);
	obj_step = (int)((float)len * 2.0 / PULSE);
	controlInterruptStart();
	
	while (1){
		stepGet();
    	speedSet(speed_target_l, speed_target_r);
		if(step_lr > obj_step){
			break;
		}
	}
	stay(init_speed);
}

void RUN::decelerate(int len, int init_speed)
{
	int obj_step;
	
	controlInterruptStop();	
	accel = SEARCH_ACCEL;
	speed = upper_speed_limit = init_speed;
	lower_speed_limit = MIN_SPEED;
	con_wall.enable = true;
	dirSet(MOT_FORWARD,MOT_FORWARD);
	speedSet(speed,speed);
	obj_step = (int)((float)len * 2.0 / PULSE);
	controlInterruptStart();
	
	while(1){
		stepGet();
    	speedSet(speed_target_l, speed_target_r);
		if( (int)(len - step_lr_len) < 
	 	    (int)(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * accel))) {
				break;
		}
	}

	accel = -1 * accel;

	while (1){
		stepGet();
  		speedSet(speed_target_l, speed_target_r);
		if(step_lr > obj_step){
			break;
		}
	}
	stop();
}

void RUN::rotate(t_local_direction dir, int times)
{
	int obj_step;
	
	controlInterruptStop();
	accel = TURN_ACCEL;
	upper_speed_limit = SEARCH_SPEED;
	speed = lower_speed_limit = MIN_SPEED;
	con_wall.enable = false;
	obj_step = (int)(TREAD_WIDTH * PI / 4.0 * (float)times * 2.0 / PULSE);
	switch (dir) {
	case right:
      dirSet(MOT_FORWARD, MOT_BACK);  
		break;
	case left:
      dirSet(MOT_BACK,MOT_FORWARD);
		break;
	default:
      dirSet(MOT_FORWARD, MOT_FORWARD);
		break;
	}
	speedSet(MIN_SPEED,MIN_SPEED);   
	counterClear();
	controlInterruptStart();

	while (1) {
	  stepGet();
      speedSet(speed_target_l, speed_target_r);
	  if ((int)((obj_step/2.0*PULSE) - step_lr_len) < (int)(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * accel))) {
	    break;
	  }
	}
	
	accel = -1.0 * TURN_ACCEL;
	
	while (1) {
	  stepGet();
      speedSet(speed_target_l, speed_target_r);
	  if (step_lr > obj_step) {
	    break;
	  }
	}

	stop();
}
