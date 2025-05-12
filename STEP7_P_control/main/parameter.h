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

#ifndef MAIN_PARAMETER_H_
#define MAIN_PARAMETER_H_


#define MIN_HZ 40
#define TIRE_DIAMETER (24.70)
#define TREAD_WIDTH (31.5)
#define PI (3.141592)
#define PULSE (TIRE_DIAMETER * PI / (35.0 / 10.0 * 20.0 * 4.0))
#define MIN_SPEED (MIN_HZ * PULSE)

#define MIN_SPEED (MIN_HZ * PULSE)

#define INC_FREQ 3000
#define DEC_FREQ 2000

//Change the value to match your environment
#define REF_SEN_R 507
#define REF_SEN_L 394

#define TH_SEN_R 250
#define TH_SEN_L 208
#define TH_SEN_FR 155
#define TH_SEN_FL 134

#define CONTH_SEN_R TH_SEN_R
#define CONTH_SEN_L TH_SEN_L

#define CON_WALL_KP (0.5)

#endif /* MAIN_PARAMETER_H_ */
