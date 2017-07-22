/**
 * StatusLEDDemo.ino - Software library for the Make-A-Pede (makeapede.com)
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

#include "MakeAPede.h"

#define OBSTACLE_LEFT (getLeftAntennae() == HIGH)
#define OBSTACLE_RIGHT (getRightAntennae() == HIGH)

// Define Sensor Pins
int leftSpeedPin = 5;
int leftDirPin = 4;
int rightSpeedPin = 6;
int rightDirPin = 7;
int leftAntennaePin = 12;
int rightAntennaePin = 13;

int red = 8;
int green = 9;
int blue = 10;

void setup() {
  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(red, green, blue);

  // Initialize the serial connection
  Serial.begin(9600);
  
  // Wait for an antenna to be triggered to start the program
  Serial.println("Press either antenna to start program");
  while (!OBSTACLE_LEFT && !OBSTACLE_RIGHT);

  // Turn off the status LED
  setRGBColor();

  delay(2000);
}

void loop() {
  // Loop through status LED settings 1-7
  for(int i=1; i<8; i++)
  {
    setRGBColor(i);
    delay(500);
  }
}

void userCode() {}
