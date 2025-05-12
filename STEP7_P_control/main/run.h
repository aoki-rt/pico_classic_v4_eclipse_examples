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

#ifndef MAIN_RUN_H_
#define MAIN_RUN_H_

#include "device.h"
#include "mytypedef.h"


class RUN
{
public:
	typedef struct
	{
	  double control;
	  double error;
	  double p_error;
	  double diff;
	  double sum;
	  double sum_max;
	  double kp;
	  double kd;
	  double ki;
	  bool enable;
	} t_control;

    t_control con_wall;

	RUN();
	void accelerate(int len, int tar_speed);
	void oneStep(int len, int tar_speed);
	void decelerate(int len, int tar_speed);
	void rotate(t_direction dir, int times);
	bool moveFlagGet(void);
	void stepIncRight(void);
	void stepIncLeft(void);
	void interruptControl(void);

private:
    volatile unsigned int step_r, step_l;
    double max_speed;
    double min_speed;
    double accel = 0.0;
    volatile double speed;

    unsigned short step_hz_r;
    unsigned short step_hz_l;

    volatile bool motor_move;
};
extern RUN g_run;
#endif /* MAIN_RUN_H_ */
