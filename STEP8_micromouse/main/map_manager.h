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

#include "mytypedef.h"

#ifndef MAIN_MAP_MANAGER_H_
#define MAIN_MAP_MANAGER_H_

#define MAZESIZE_X (16)  //maze x size(MAZESIZE_X * MAZESIZE_Y)
#define MAZESIZE_Y (16)  //maze y size(MAZESIZE_Y * MAZESIZE_Y)

#define _UNKNOWN 2  //wall information unknown
#define NOWALL 0    //wall information Wall No
#define WALL 1      //wall information Wall Yes
#define VWALL 3     //wall information Temporary wall(unused)

typedef struct
{
	unsigned char west : 2;   //west wall information bit7-6
	unsigned char south : 2;  //south wall information bit5-4
	unsigned char east : 2;   //east wall information bit3-2
	unsigned char north : 2;  //north wall information bit1-0
} t_wall;                   //wall information structure(bit field)

typedef enum { north, east, south, west } t_direction_glob;

typedef struct
{
	unsigned char x;
	unsigned char y;
	t_direction_glob dir;
} t_position;

class MapManager
{
public:
	MapManager();

	t_position mypos;
	short goal_mx = 7;
	short goal_my = 7;

	void positionInit(void);
	char wallDataGet(unsigned char x, unsigned char y, t_direction_glob dir);
	void wallDataSet(unsigned char x, unsigned char y, t_direction_glob dir, char data);

	void axisUpdate(void);
	void rotateDirSet(t_direction dir);
	void wallSet(bool IS_SEN_FR, bool IS_SEN_R, bool IS_SEN_L);
	t_direction nextDirGet(char x, char y, t_direction_glob * dir);
	t_direction nextDir2Get(short x, short y, t_direction_glob * dir);

private:
	unsigned short steps_map[MAZESIZE_X][MAZESIZE_Y];  //step map
	t_wall wall[MAZESIZE_X][MAZESIZE_Y];               //wall information structure arrays
	void searchMapMake(int x, int y);
	void map2Make(int x, int y);
	int priorityGet(unsigned char x, unsigned char y, t_direction_glob dir);
};


extern MapManager g_map_control;

#endif /* MAIN_MAP_MANAGER_H_ */
