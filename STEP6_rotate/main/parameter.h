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


#define TIRE_DIAMETER (24.7)
#define TREAD_WIDTH (31.5)
#define TREAD_CIRCUIT (TREAD_WIDTH * PI / 4)
#define PI	(3.141592)
#define PULSE (TIRE_DIAMETER * PI / (35.0 / 10.0 * 20.0 * 8.0))
#define MIN_HZ 40

#define MIN_SPEED (MIN_HZ * PULSE)

#define INC_FREQ 3000
#define DEC_FREQ 2000

#endif /* MAIN_PARAMETER_H_ */
