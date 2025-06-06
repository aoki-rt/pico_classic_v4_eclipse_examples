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

#include "parameter.h"
#include "TMC5240.h"
#include "run.h"

RUN g_run;


extern "C" void controlInterrupt(void){
	g_run.interrupt();
}


RUN::RUN(){
    accel = 0.0;
    speed = 0.0;
}

void RUN::interrupt(void){
	speed = speed + accel;

	if(speed > max_speed){
		speed = max_speed;
	}
	if(speed < min_speed){
		speed = min_speed;
	}
}

void RUN::dirSet(t_CW_CCW dir_left, t_CW_CCW dir_right)
{
  g_tmc5240.write(TMC5240_RAMPMODE, dir_left, dir_right);
}

void RUN::counterClear(void) { g_tmc5240.write(TMC5240_XACTUAL, 0, 0); }

void RUN::speedSet(double l_speed, double r_speed)
{
  g_tmc5240.write(
    TMC5240_VMAX, (unsigned int)(l_speed / TMC5240_VELOCITY),
    (unsigned int)(r_speed / TMC5240_VELOCITY));
}

void RUN::stepGet(void)
{
  step_lr = g_tmc5240.readXactual();
  step_lr_len = (int)((float)step_lr / 2.0 * PULSE);
}

void RUN::stop(void) { g_tmc5240.write(TMC5240_VMAX, 0, 0); }


void RUN::accelerate(int len, int finish_speed)
{
	int obj_step;

	accel = 1.5;
	speed = MIN_SPEED;
	min_speed = MIN_SPEED;
	max_speed = finish_speed;
	counterClear();	
	speedSet(MIN_SPEED,MIN_SPEED);
	dirSet(MOT_FORWARD,MOT_FORWARD);
	obj_step = (int)((float)len * 2.0 /PULSE);

	while (1){
		stepGet();
		speedSet(speed,speed);
		if(step_lr > obj_step){
			break;
		}
	}
}

void RUN::oneStep(int len, int init_speed)
{
	int obj_step;
	
	accel = 0.0;	
	max_speed = init_speed;
	speed = init_speed;
	min_speed = init_speed;
	counterClear();	
	speedSet(init_speed,init_speed);
	dirSet(MOT_FORWARD,MOT_FORWARD);
	obj_step = (int)((float)len * 2.0 / PULSE);

	while (1){
		stepGet();
		speedSet(speed,speed);
		if(step_lr > obj_step){
			break;
		}
	}
}

void RUN::decelerate(int len, int init_speed)
{
	int obj_step;
	
	accel = 1.5;
	max_speed = init_speed;
	speed = init_speed;
	min_speed = init_speed;
	counterClear();	
	speedSet(init_speed,init_speed);
	dirSet(MOT_FORWARD,MOT_FORWARD);
	obj_step = (int)((float)len * 2.0 / PULSE);

	while(1){
		stepGet();
		speedSet(speed,speed);
		if( (int)(len - step_lr_len) < 
	 	    (int)(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * accel))) {
				break;
		}
	}

	accel = -1.5;
	min_speed = MIN_SPEED;

	while (1){
		stepGet();
		speedSet(speed,speed);
		if(step_lr > obj_step){
			break;
		}
	}
	stop();
}
