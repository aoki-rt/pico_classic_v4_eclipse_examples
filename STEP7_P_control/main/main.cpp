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

#include <iostream>

#include "device.h"
#include "run.h"
#include "sensor.h"

using namespace std;

extern "C" void app_main(void)
{
	char temp;
	g_device.allInit();

    while (true) {
    	while(true){
    		temp = g_device.switchGet();
    		if(temp!=0){
    			break;
    		}
    		delay(10);
    	}

    	if (temp == SW_RM) {
    	    while (1) {
    	    	std::cout<<"r_sen  is  " << g_sensor.sen_r.value << std::endl;
    	    	std::cout<<"fr_sen  is  " << g_sensor.sen_fr.value << std::endl;
    	    	std::cout<<"fl_sen  is  " << g_sensor.sen_fl.value << std::endl;
    	    	std::cout<<"l_sen  is  " << g_sensor.sen_l.value << std::endl;
    	    	std::cout<<"VDD  is  " << g_sensor.battery_value << std::endl;
    	    	delay(100);
    	    }
    	}
    	g_device.motorEnable();
    	delay(1000);
    	g_run.accelerate(45, 200);
    	g_run.oneStep(90 * 3, 200);
    	g_run.decelerate(45, 200);
    	delay(1000);
    	g_device.motorDisable();
    }
}
