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

//フラッシュの初期化の確認をする場合はdefineのコメントを外す
//#define FLASH_CHECK


extern "C"
{
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_spiffs.h"
#include "device.h"
}

#include <iostream>
#include <string>
#include <fstream>

#include "flash.h"

#include "map_manager.h"

FLASH g_flash;



void spiffsBegin(void){

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 5,
		.format_if_mount_failed = true
	};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
        	printf("Failed to find SPIFFS partition");
        } else {
        	printf("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

#ifdef FLASH_CHECK
    printf("Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);
    if (ret != ESP_OK) {
    	printf("SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return;
    } else {
    	printf("SPIFFS_check() successful");
    }


    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
    	printf("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return;
    } else {
        printf("Partition size: total: %d, used: %d", total, used);
    }

    if (used > total) {
    	printf("Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);

        if (ret != ESP_OK) {
        	printf("SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return;
        } else {
        	printf("SPIFFS_check() successful");
        }
    }
#endif
}

void FLASH::mapCopy(void){
	controlInterruptStop();
	sensorInterruptStop();

	char read_data[1];

	std::ifstream inputfile;
	inputfile.open("/spiffs/map.txt");

	if(inputfile.fail()){
		std::cout<<"- failed to open file for reading"<<std::endl;
		return;
	}
	for (int i = 0; i < MAZESIZE_X; i++) {
		for (int j = 0; j < MAZESIZE_Y; j++) {
			if(!inputfile.eof()){
				inputfile.read(read_data,sizeof(read_data));
				g_map.wallDataSet(i, j, north, ((unsigned char)read_data[0]) & 0x03);
				g_map.wallDataSet(i, j, east, ((unsigned char)read_data[0] >> 2) & 0x03);
				g_map.wallDataSet(i, j, south, ((unsigned char)read_data[0] >> 4) & 0x03);
				g_map.wallDataSet(i, j, west, ((unsigned char)read_data[0] >> 6) & 0x03);
			} else {
				std::cout<<"Read Error"<<std::endl;
			}
		}
	}
	inputfile.close();
	controlInterruptStart();
	sensorInterruptStart();
}

void FLASH::mapWrite(void){
	unsigned char data_temp;

	controlInterruptStop();
	sensorInterruptStop();

    std::ofstream outputfile;
    outputfile.open("/spiffs/map.txt");

    if (outputfile.fail()) {
    	std::cout<<"- failed to open file for writing"<<std::endl;
    	return;
    }
    for (int i = 0; i < MAZESIZE_X; i++) {
    	for (int j = 0; j < MAZESIZE_Y; j++) {
    		data_temp = g_map.wallDataGet(i, j, north) +
    				(g_map.wallDataGet(i, j, east) << 2) +
					(g_map.wallDataGet(i, j, south) << 4) +
					(g_map.wallDataGet(i, j, west) << 6);
    		if (outputfile<<data_temp) {  //binary data write
    		} else {
    			std::cout<<"- write failed"<<std::endl;
    		}
    	}
    }

    outputfile.close();
	controlInterruptStart();
	sensorInterruptStart();

}

