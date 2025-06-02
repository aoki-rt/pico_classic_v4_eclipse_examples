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

#ifndef MAIN_PARAMETER_H_
#define MAIN_PARAMETER_H_


#define TIRE_DIAMETER (48.0)
#define TREAD_WIDTH (64.0)
#define TREAD_CIRCUIT (TREAD_WIDTH * PI / 4)
#define PI (3.141592)
#define MIN_SPEED 30

//Change the value to match your environment
#define REF_SEN_R 552
#define REF_SEN_L 327

#define TH_SEN_R 173
#define TH_SEN_L 169
#define TH_SEN_FR 145
#define TH_SEN_FL 134

#define CONTH_SEN_R TH_SEN_R
#define CONTH_SEN_L TH_SEN_L

#define CON_WALL_KP (0.5)

#endif /* MAIN_PARAMETER_H_ */
