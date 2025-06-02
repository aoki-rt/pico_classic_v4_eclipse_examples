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


extern "C"{
	#include "device.h"
	#include "stdio.h"
}
#include "run.h"
#include "sensor.h"
#include "TMC5240.h"


extern "C" void app_main(void)
{
	char temp;
	allInit();
	motorEnable();
	delay(1);
	g_tmc5240.init();
	motorDisable();

    while (true) {
    	while(true){
    		temp = switchGet();
    		if(temp!=0){
    			break;
    		}
    		delay(10);
    	}

    	if (temp == SW_LM) {
    	    while (true) {
				printf("r_sen	is %d\n\r", g_sensor.sen_r.value);
				printf("fr_sen	is %d\n\r", g_sensor.sen_fr.value);
				printf("fl_sen	is %d\n\r", g_sensor.sen_fl.value);
				printf("l_sen	is %d\n\r", g_sensor.sen_l.value);
				printf("VDD	is %dmV\n\r", g_sensor.battery_value);
    	    	delay(100);
    	    }
    	}
    	motorEnable();
    	delay(1000);
    	g_run.accelerate(90, 350);
    	g_run.oneStep(180 * 3, 350);
    	g_run.decelerate(90, 350);
    	delay(1000);
    	motorDisable();
    }
}
