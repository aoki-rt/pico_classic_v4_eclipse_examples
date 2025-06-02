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

#include "TMC5240.h"

extern "C"{
	#include "device.h"
}

TMC5240 g_tmc5240;


unsigned int TMC5240::readXactual(void)
{
  int data_r, data_l;
  data_r=data_l=0;
    
  spiRead32(TMC5240_XACTUAL | TMC5240_READ, &data_l, SPI_CS_L);
  spiRead32(TMC5240_XACTUAL | TMC5240_READ, &data_l, SPI_CS_L);

    for (int i = 0; i < 0xff; i++) {
    asm("nop \n");
  }

  spiRead32(TMC5240_XACTUAL | TMC5240_READ,&data_r, SPI_CS_R);
  spiRead32(TMC5240_XACTUAL | TMC5240_READ,&data_r, SPI_CS_R);
  
  if(data_l<0)data_l = data_l * -1;
  if(data_r<0)data_r = data_r * -1;

  return (unsigned int)(data_r + data_l);
}

void TMC5240::write(unsigned char add, unsigned int data_l, unsigned int data_r)
{
  spiWrite32(add | TMC5240_WRITE,(int)data_l,SPI_CS_L);
  spiWrite32(add | TMC5240_WRITE,(int)data_r,SPI_CS_R);
}


void TMC5240::init(void)
{
  g_tmc5240.write(TMC5240_DRV_CONF, 0x00000031, 0x00000031);  //Current Range 2A 800V/us
  g_tmc5240.write(TMC5240_IHOLD_IRUN, 0x01041C03,0x01041C03);  //IHOLDDELAY=4 IRUN=28/32 IHOLD=3/32 2A設定時ピーク1.5A、実効値1.06A
  g_tmc5240.write(TMC5240_CHOPCONG, 0x04000001,0x04000001);  //MRES=0 1/16step TOFFTime=1 1以上でないとモータが動作しない
  g_tmc5240.write(TMC5240_PWMCONF, 0xC40E001D, 0xC40E001D);   //PWM 48.8kHz
  g_tmc5240.write(TMC5240_RAMPMODE, 0x00000001, 0x00000001);  //velocity mode(positive)
  g_tmc5240.write(TMC5240_GCONF, 0x00000000, 0x00000010);     //右のみシャフトインバース
  g_tmc5240.write(TMC5240_AMAX, 0x3ffff, 0x3ffff);            //加速度max 17..0 18bit
  g_tmc5240.write(TMC5240_XACTUAL, 0, 0);                     //初期化
  g_tmc5240.write(TMC5240_VSTART, 810, 810);                   //16microstep min_speed=30の時の値 
  g_tmc5240.write(TMC5240_VMAX, 0, 0);
 
}