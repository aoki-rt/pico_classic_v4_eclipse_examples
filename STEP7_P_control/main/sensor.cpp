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


extern "C"{
	#include "device.h"
}

#include "sensor.h"
#include "parameter.h"

SENSOR g_sensor;


extern "C" void sensorInterrupt(void){
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
			sen_fr.value = sensorGetFR();
			if (sen_fr.value > sen_fr.th_wall) {
				sen_fr.is_wall = true;
			} else {
				sen_fr.is_wall = false;
			}
			break;
		case 1:
			sen_fl.value = sensorGetFL();
			if (sen_fl.value > sen_fl.th_wall) {
				sen_fl.is_wall = true;
			} else {
				sen_fl.is_wall = false;
			}
			break;
		case 2:
			sen_r.value = sensorGetR();
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
			break;
		case 3:
			sen_l.value = sensorGetL();
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

		    battery_value = batteryVoltGet();

			break;
	    default:
	    	break;
	  	}
	  	cnt++;
	  	if (cnt >= 4) cnt = 0;
}


