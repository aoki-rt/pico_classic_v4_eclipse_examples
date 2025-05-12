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
#include <iomanip>
#include "adjust.h"
#include "device.h"
#include "map_manager.h"
#include "misc.h"
#include "sensor.h"
#include "run.h"

#include "flash.h"


ADJUST g_adjust;

void ADJUST::mapView(void)
{

  std::cout << "\x1b[2j";
  std::cout << "\x1b[0;0H" <<std::endl;
  std::cout << "+";
  for (int i = 0; i < MAZESIZE_X; i++) {
    switch (g_map_control.wallDataGet(i, MAZESIZE_Y - 1, north)) {  //black color is"[30m"
      case NOWALL:
        std::cout << "\x1b[37m  +";  //NOWALL
        break;
      case WALL:
    	  std::cout << "\x1b[37m--+";  //WALL
        break;
      case _UNKNOWN:
    	  std::cout << "\x1b[31m--+";  //UNNOWN
        break;
      default:
    	  std::cout << "\x1b[33m--+";  //VWALL
        break;
    }
  }
  std::cout<<std::endl;
  for (int j = (MAZESIZE_Y - 1); j > -1; j--) {
    switch (g_map_control.wallDataGet(0, j, west)) {
      case NOWALL:
    	  std::cout << "\x1b[37m ";  //NOWALL
        break;
      case WALL:
    	  std::cout << "\x1b[37m|";  //WALL
        break;
      case _UNKNOWN:
    	  std::cout << "\x1b[31m|";  //UNNOWN
        break;
      default:
    	  std::cout << "\x1b[33m|";  //VWALL
        break;
    }
    for (int i = 0; i < MAZESIZE_X; i++) {
      switch (g_map_control.wallDataGet(i, j, east)) {
        case NOWALL:
        	std::cout << "\x1b[37m   ";  //NOWALL
          break;
        case WALL:
        	std::cout << "\x1b[37m  |";  //WALL
          break;
        case _UNKNOWN:
        	std::cout << "\x1b[31m  |";  //UNNOWN
          break;
        default:
        	std::cout << "\x1b[33m  |";  //VWALL
          break;
      }
    }
    std::cout<<std::endl<<"+";
    for (int i = 0; i < MAZESIZE_X; i++) {
      switch (g_map_control.wallDataGet(i, j, south)) {
        case NOWALL:
        	std::cout << "\x1b[37m  +";  //NOWALL
          break;
        case WALL:
        	std::cout << "\x1b[37m--+";  //WALL
          break;
        case _UNKNOWN:
        	std::cout << "\x1b[31m--+";  //UNNOWN
          break;
        default:
        	std::cout << "\x1b[33m--+";  //VWALL
          break;
      }
    }
    std::cout<<std::endl;
  }
}

void ADJUST::adcValueView(void)
{
	g_device.motorDisable();

	while (1) {
		std::cout <<"r_sen        is " << std::setw(6) << g_sensor.sen_r.value << std::endl;
		std::cout <<"fr_sen       is " << std::setw(6) << g_sensor.sen_fr.value << std::endl;
		std::cout <<"fl_sen       is " << std::setw(6) << g_sensor.sen_fl.value << std::endl;
		std::cout <<"l_sen        is " << std::setw(6) << g_sensor.sen_l.value << std::endl;
		std::cout <<"VDD          is " << std::setw(6) << g_sensor.battery_value << "mv" << std::endl;
		std::cout<<std::endl;
		delay(100);
	}
}

void ADJUST::straightCheck(int section_check)
{
	g_device.motorEnable();
	delay(1000);
	g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
	if (section_check > 1) {
		g_run.straight(SECTION * (section_check - 1), SEARCH_SPEED, MAX_SPEED, SEARCH_SPEED);
	}
	g_run.decelerate(HALF_SECTION, SEARCH_SPEED);

	g_device.motorDisable();
}

void ADJUST::rotationCheck(void)
{
	g_device.motorEnable();
	delay(1000);
	for (int i = 0; i < 8; i++) {
		g_run.rotate(right, 1);
	}
	g_device.motorDisable();
}

void ADJUST::menu(void)
{
	unsigned char mode = 1;
	char LED3_data;
	char sw;

	while (1) {
		g_device.LEDSet(mode);
		while (1) {
			sw = g_device.switchGet();
			if (sw != 0) break;
			delay(33);
			LED3_data ^= 1;
			g_device.LEDSet((mode & 0x7) + ((LED3_data << 3) & 0x08));
		}
		LED3_data = 0;
		switch (sw) {
      		case SW_RM:
      			mode = g_misc.buttonInc(mode, 7, 1);
      			break;
      		case SW_LM:
      			g_misc.buttonOk();
      			if (modeExec(mode) == 1) {
      				return;
      			}
      			break;
      		default:
      			break;
		}
	}
}

unsigned char ADJUST::modeExec(unsigned char mode)
{
  g_device.motorEnable();
  switch (mode) {
    case 1:
      adcValueView();
      break;
    case 2:
      straightCheck(9);
      break;

    case 3:
      rotationCheck();
      break;
    case 4:
      g_flash.mapCopy();
      mapView();
      break;

    case 5:
      break;

    case 6:
      break;

    default:
      return 1;
      break;
  }

  return 0;
}

