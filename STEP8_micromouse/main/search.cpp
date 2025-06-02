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

#include "search.h"

#include "run.h"
#include "sensor.h"
#include "map_manager.h"
#include "parameter.h"

SEARCH g_search;

void SEARCH::lefthand(void)
{
  g_run.accelerate(HALF_SECTION, SEARCH_SPEED);

  while (1) {
    if (g_sensor.sen_l.is_wall == false) {
      g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
      g_run.rotate(left, 1);
      g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
    } else if ((g_sensor.sen_fl.is_wall == false) && (g_sensor.sen_fr.is_wall == false)) {
      g_run.straight(SECTION, SEARCH_SPEED, SEARCH_SPEED, SEARCH_SPEED);
    } else if (g_sensor.sen_r.is_wall == false) {
      g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
      g_run.rotate(right, 1);
      g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
    } else {
      g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
      g_run.rotate(right, 2);
      g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
    }
  }
}

void SEARCH::adachi(char gx, char gy)
{
  t_global_direction glob_nextdir;

  switch (g_map.nextDirGet(gx, gy, &glob_nextdir)) {
    case front:
      break;
    case right:
      g_run.rotate(right, 1);
      break;
    case left:
      g_run.rotate(left, 1);
      break;
    case rear:
      g_run.rotate(right, 2);
      break;
	default:
	  break;
  }

  g_run.accelerate(HALF_SECTION, SEARCH_SPEED);

  g_map.mypos.dir = glob_nextdir;
  g_map.axisUpdate();

  while ((g_map.mypos.x != gx) || (g_map.mypos.y != gy)) {
    g_map.wallSet(g_sensor.sen_fr.is_wall, g_sensor.sen_r.is_wall, g_sensor.sen_l.is_wall);

    switch (g_map.nextDirGet(gx, gy, &glob_nextdir)) {
      case front:
        g_run.oneStep(SECTION, SEARCH_SPEED);
        break;
      case right:
        g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
        g_run.rotate(right, 1);
        g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
        break;
      case left:
        g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
        g_run.rotate(left, 1);
        g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
        break;
      case rear:
        g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
        g_run.rotate(right, 2);
        g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
        break;
	  default:
		break;
    }

    g_map.mypos.dir = glob_nextdir;  //dir update
    g_map.axisUpdate();
  }

  g_map.wallSet(g_sensor.sen_fr.is_wall, g_sensor.sen_r.is_wall, g_sensor.sen_l.is_wall);
  g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
}

