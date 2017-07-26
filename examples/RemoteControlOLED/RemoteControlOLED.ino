/**
 * RemoteControlOLED.ino - Software library for the Make-A-Pede (makeapede.com)
 * Copyright (C) 2017 Automata-Development
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * This example requires 2 OLED displays to be installed
 * 
 * Open the MakeAPede.h file and uncomment the "#define USE_DISPLAY" line
 */
#include "MakeAPede.h"

// Define Sensor Pins
int leftSpeedPin = 5;
int leftDirPin = 4;

int rightSpeedPin = 6;
int rightDirPin = 7;

int leftAntennaePin = 12;
int rightAntennaePin = 13;

// Define variable to store the current state of the eyes
bool eyesOpen = true;

// Define variables to store the current time and the time of the last blink
unsigned long currentTime = 0;
unsigned long blinkTime = 0; 

void setup() {
  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  
  enableObstacleAvoid(true);
}

void loop() { bluetoothControl(); }

void userCode() {
  // Read the current time
  currentTime = millis();

  // Close the eyes if either antenna is pressed
  if (getLeftAntennae() == HIGH || getRightAntennae() == HIGH) {
    if (eyesOpen) {
      // Display closed eyes
      showClosedEyes();
  
      // Save the current state
      eyesOpen = false;
    }
  }
  
  // If 4 seconds have passed since the last blink and the eyes are open
  else if (((currentTime - blinkTime) > 6000) && eyesOpen) {
    // Display closed eyes
    showClosedEyes();

    // Save the current time and state
    eyesOpen = false;
    blinkTime = currentTime;
  }
  
  // If 0.5 seconds have passed since the last blink and the eyes are closed
  else if (((currentTime - blinkTime) > 200) && !eyesOpen) {
    // Display open eyes
    showEyes();

    // Save the current time and state
    eyesOpen = true;
    blinkTime = currentTime;
  }
}

