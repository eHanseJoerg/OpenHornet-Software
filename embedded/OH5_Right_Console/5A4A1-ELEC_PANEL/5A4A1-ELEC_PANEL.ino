/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *   Copyright 2016-2024 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *   ----------------------------------------------------------------------------------
 *   Note: All other portions of OpenHornet not within the 'OpenHornet-Software' 
 *   GitHub repository is released under the Creative Commons Attribution -
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/

/**
 * @file      5A4A1-ELEC_PANEL.ino
 * @author    Ulukaii
 * @date      04.August 2025
 * @version   t.0.3.4
 * @copyright Copyright 2016-2025 OpenHornet. Licensed under the Apache License, V 2.0.
 * @brief     Controls the ELEC panel on the right console, including the batt gauge,
 *
 * @details
 * 
 *  * **Reference Designator:** 5A4A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 3
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Left Generator
 * 2   | Battery ON
 * A2  | Battery OVRD
 * 3   | Right Generator
 * A1  | Brake Pressure Stepper M4
 * 15  | Battery gauge coil 1 
 * 6   | Battery gauge coil 2
 * 14  | Battery gauge coil 3
 * 7   | Battery gauge coil 4
 * 16  | Battery gauge coil 5
 * 8   | Battery gauge coil 6
 * 10  | Battery gauge coil 7
 * 9   | Battery gauge coil 8
*/


/**********************************************************************************************************************
 * @brief  Preprocessor directives, library includes, and pin definitions
 * @note   Activate/deactivate RS485 usage here. 

 *********************************************************************************************************************/
//#define DCSBIOS_RS485_SLAVE 3
#define DCSBIOS_DEFAULT_SERIAL  //comment out for RS 485
#ifdef __AVR__
#include <avr/power.h>
#endif
//#define TXENABLE_PIN 5  ///< Sets TXENABLE_PIN to Arduino Pin 5
//#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"
#include <Stepper.h>
#include <Wire.h>
#include "HornetStepper.h"

#define L_GEN A3                                                      // < Left Generator
#define BAT_ON 2                                                      // < Battery ON
#define BAT_OVRD A2                                                   // < Battery OVRD
#define R_GEN 3                                                       // < Right Generator
#define COIL_1 15                                                     // < Battery gauge coil 1 (E gauge)
#define COIL_2 6                                                      // < Battery gauge coil 2 (E gauge)
#define COIL_3 14                                                     // < Battery gauge coil 3 (E gauge)
#define COIL_4 7                                                      // < Battery gauge coil 4 (E gauge)
#define COIL_5 16                                                     // < Battery gauge coil 5 (U gauge)
#define COIL_6 8                                                      // < Battery gauge coil 6 (U gauge)
#define COIL_7 10                                                     // < Battery gauge coil 7 (U gauge)
#define COIL_8 9                                                      // < Battery gauge coil 8 (U gauge)


//Connect switches to DCS-BIOS
DcsBios::Switch2Pos rGenSw("R_GEN_SW", R_GEN, true);
DcsBios::Switch2Pos lGenSw("L_GEN_SW", L_GEN, true);
DcsBios::Switch3Pos batterySw("BATTERY_SW", BAT_OVRD, BAT_ON);

// Initialize Hornet stepper objects (Parameters: stepsPerRevolution, 
// zeroPosition, maxPosition, directionForward, coils)
// You NEED TO calibrate the first four parameters to your specific stepper!
HornetStepper eStepper(310, 5, 250, 1, COIL_1, COIL_2, COIL_3, COIL_4);  // E gauge
HornetStepper uStepper(310, 5, 240, 1, COIL_5, COIL_6, COIL_7, COIL_8);  // U gauge


// Callback functions for DCS-BIOS (we'll handle these in the next step)
void onVoltEChange(unsigned int newValue) {
    eStepper.setTarget(newValue);
}
DcsBios::IntegerBuffer voltEBuffer(FA_18C_hornet_VOLT_E, onVoltEChange);

void onVoltUChange(unsigned int newValue) {
    uStepper.setTarget(newValue);
}
DcsBios::IntegerBuffer voltUBuffer(FA_18C_hornet_VOLT_U, onVoltUChange);



void setup() {
  eStepper.findZero();
  eStepper.startupTest();
  uStepper.findZero();
  uStepper.startupTest();
  DcsBios::setup(); 
}

void loop() {
  eStepper.setNeedle();
  uStepper.setNeedle();
  DcsBios::loop();
}



