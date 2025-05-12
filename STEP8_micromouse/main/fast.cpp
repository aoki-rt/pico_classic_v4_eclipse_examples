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

#include "fast.h"

#include "run.h"
#include "map_manager.h"

FAST g_fast;

void FAST::run(short gx, short gy)
{
  t_direction_glob glob_nextdir;
  int straight_count = 0;

  t_direction temp_next_dir = g_map_control.nextDir2Get(gx, gy, &glob_nextdir);
  switch (temp_next_dir) {
    case right:
    	g_run.rotate(right, 1);  //right turn
      break;
    case left:
    	g_run.rotate(left, 1);  //left turn
      break;
    case rear:
    	g_run.rotate(right, 2);  //180deg turn
      break;
    default:
      break;
  }

  g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
  straight_count = 0;
  g_map_control.mypos.dir = glob_nextdir;
  g_map_control.axisUpdate();

  while ((g_map_control.mypos.x != gx) || (g_map_control.mypos.y != gy)) {
    switch (g_map_control.nextDir2Get(gx, gy, &glob_nextdir)) {
      case front:
        straight_count++;
        break;
      case right:
        g_run.straight(straight_count * SECTION, SEARCH_SPEED, MAX_SPEED, SEARCH_SPEED);
        straight_count = 0;
        g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
        g_run.rotate(right, 1);
        g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
        break;
      case left:
        g_run.straight(straight_count * SECTION, SEARCH_SPEED, MAX_SPEED, SEARCH_SPEED);
        straight_count = 0;
        g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
        g_run.rotate(left, 1);
        g_run.accelerate(HALF_SECTION, SEARCH_SPEED);
        break;
      default:
        break;
    }
    g_map_control.mypos.dir = glob_nextdir;
    g_map_control.axisUpdate();
  }
  if (straight_count > 0) {
    g_run.straight(straight_count * SECTION, SEARCH_SPEED, MAX_SPEED, SEARCH_SPEED);
  }
  g_run.decelerate(HALF_SECTION, SEARCH_SPEED);
}

