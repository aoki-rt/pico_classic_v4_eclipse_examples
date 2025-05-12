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

#include "sensor.h"
#include "device.h"
#include "parameter.h"

SENSOR g_sensor;


void sensorInterruptControl(void){
	g_sensor.interrupt();
}

SENSOR::SENSOR()
{
	sen_r.ref = REF_SEN_R;
	sen_l.ref = REF_SEN_L;

	sen_r.th_wall = TH_SEN_R;
	sen_l.th_wall = TH_SEN_L;

	sen_fr.th_wall = TH_SEN_FR;
	sen_fl.th_wall = TH_SEN_FL;

	sen_r.th_control = CONTH_SEN_R;
	sen_l.th_control = CONTH_SEN_L;

}


void SENSOR::interrupt(void)
{
	static char cnt = 0;

	switch (cnt) {
		case 0:
			g_device.sensorGetF(&sen_fl.value,&sen_fr.value);

			if (sen_fr.value > sen_fr.th_wall) {
				sen_fr.is_wall = true;
			} else {
				sen_fr.is_wall = false;
			}
			if (sen_fl.value > sen_fl.th_wall) {
				sen_fl.is_wall = true;
			} else {
				sen_fl.is_wall = false;
			}
			break;
		case 1:
			g_device.sensorGetS(&sen_l.value,&sen_r.value);

			if (sen_r.value > sen_r.th_wall) {
				sen_r.is_wall = true;
			} else {
				sen_r.is_wall = false;
			}
			if (sen_r.value > sen_r.th_control) {
				sen_r.error = sen_r.value - sen_r.ref;
				sen_r.is_control = true;
			} else {
				sen_r.error = 0;
				sen_r.is_control = false;
			}
			if (sen_l.value > sen_l.th_wall) {
				sen_l.is_wall = true;
			} else {
				sen_l.is_wall = false;
			}
			if (sen_l.value > sen_l.th_control) {
				sen_l.error = sen_l.value - sen_l.ref;
				sen_l.is_control = true;
			} else {
				sen_l.error = 0;
				sen_l.is_control = false;
			}

		    battery_value = (double)g_device.voltageGet() / 1.0 * (1.0 + 10.0);

			break;
	    default:
	    	break;
	  	}
	  	cnt++;
	  	if (cnt == 2) cnt = 0;
}


