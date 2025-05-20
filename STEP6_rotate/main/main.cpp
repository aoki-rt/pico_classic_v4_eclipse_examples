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
extern "C" {
#include "device.h"
}
#include "run.h"
#include "TMC5240.h"

extern "C" void app_main(void)
{
	allInit();
	motorEnable();
	delay(1);
	g_tmc5240.init();
	motorDisable();

    while (true) {
    	while(switchGet()==0) {
    		delay(10);
    	    continue;
    	}
    	motorEnable();
    	delay(1000);
    	g_run.rotate(right, 1);
    	delay(1000);
    	g_run.rotate(left, 1);
    	delay(1000);
    	g_run.rotate(right, 2);
    	delay(1000);
    	g_run.rotate(left, 2);
    	delay(1000);
    	motorDisable();
    }

}
