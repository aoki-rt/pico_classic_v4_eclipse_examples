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
}
#include "adjust.h"
#include "fast.h"
#include "flash.h"
#include "map_manager.h"
#include "misc.h"
#include "parameter.h"
#include "search.h"
#include "run.h"

MISC g_misc;

short MISC::buttonInc(short data, short limit, short limit_data)
{
	data++;
	if (data > limit) {
		data = limit_data;
	}
	buzzerEnable(INC_FREQ);
	delay(30);
	buzzerDisable();
	return data;
}

short MISC::buttonDec(short data, short limit, short limit_data)
{
  data--;
  if (data < limit) {
    data = limit_data;
  }
  buzzerEnable(DEC_FREQ);
  delay(30);
  buzzerDisable();
  return data;
}

void MISC::buttonOk(void)
{
	buzzerEnable(DEC_FREQ);
	delay(80);
	buzzerEnable(INC_FREQ);
	delay(80);
	buzzerDisable();
}

void MISC::goalAppeal(void)
{
	unsigned char led_data;

	int wtime = 100;

	motorDisable();
	g_flash.mapWrite();

	for (int j = 0; j < 10; j++) {
		led_data = 1;
		for (int i = 0; i < 4; i++) {
			ledSet(led_data);
			led_data <<= 1;
			delay(wtime);
		}
		led_data = 8;
		for (int i = 0; i < 4; i++) {
			ledSet(led_data);
			led_data >>= 1;
			delay(wtime);
		}
		wtime -= 5;
	}
	delay(500);
	motorEnable();
}


void MISC::modeExec(int mode)
{
	motorEnable();
	delay(1000);

	switch (mode) {
    	case 1:
    		g_search.lefthand();
    		break;
    	case 2:  //adach method
    		g_map.positionInit();
			g_search.adachi(g_map.goal_mx, g_map.goal_my);
			g_run.rotate(right, 2);
			g_map.rotateDirSet(right);
			g_map.rotateDirSet(right);
			g_misc.goalAppeal();
			g_search.adachi(0, 0);
			g_run.rotate(right, 2);
			g_map.rotateDirSet(right);
			g_map.rotateDirSet(right);
			g_flash.mapWrite();
			break;
    	case 3:  //shortest running
			g_flash.mapCopy();
			g_map.positionInit();
			g_fast.run(g_map.goal_mx, g_map.goal_my);
			g_run.rotate(right, 2);
			g_map.rotateDirSet(right);
			g_map.rotateDirSet(right);
			g_misc.goalAppeal();
			g_fast.run(0, 0);
			g_run.rotate(right, 2);
			g_map.rotateDirSet(right);
			g_map.rotateDirSet(right);
			break;
    	case 4:
    		break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			motorDisable();
			g_adjust.menu();  //to adjust menu
			break;
		default:
			break;
	}
	motorDisable();
}


