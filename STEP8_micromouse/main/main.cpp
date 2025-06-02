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
#include "adjust.h"
#include "fast.h"
#include "flash.h"
#include "run.h"
#include "search.h"
#include "sensor.h"
#include "map_manager.h"
#include "misc.h"
#include "parameter.h"
#include "TMC5240.h"


signed char g_mode;

extern "C" void app_main(void)
{
	delay(1000);
	spiffsBegin();
	allInit();
	g_map.goal_mx = GOAL_X;
	g_map.goal_my = GOAL_Y;

	motorEnable();
	buzzerEnable(INC_FREQ);
	delay(80);
	g_tmc5240.init();
	motorDisable();
	buzzerDisable();
	
	g_misc.mode_select = 1;

    while (true) {
		ledSet(g_misc.mode_select);
		switch (switchGet()) {
		  case SW_RM:
		    g_misc.mode_select = g_misc.buttonInc(g_misc.mode_select, 15, 1);
		    break;
		  case SW_LM:
		    g_misc.mode_select = g_misc.buttonDec(g_misc.mode_select, 1, 15);
		    break;
		  case SW_CM:
		    g_misc.buttonOk();
		    g_misc.modeExec(g_misc.mode_select);
		    break;
		}
    	delay(10);
    }
}
