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

#ifndef MAIN_SENSOR_H_
#define MAIN_SENSOR_H_

#include "device.h"

typedef struct
{
	short value;
	short p_value;
	short error;
	short ref;
	short th_wall;
	short th_control;
	bool is_wall;
	bool is_control;
} t_sensor;

class SENSOR
{
public:
	typedef struct
	{
	  short value;
	  short p_value;
	  short error;
	  short ref;
	  short th_wall;
	  short th_control;
	  bool is_wall;
	  bool is_control;
	} t_sensor;

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

	t_sensor sen_r, sen_l, sen_fr, sen_fl;
	t_control con_wall;
	volatile short battery_value;

	SENSOR();
	void interrupt(void);


private:
};

extern SENSOR g_sensor;


#endif /* MAIN_SENSOR_H_ */
