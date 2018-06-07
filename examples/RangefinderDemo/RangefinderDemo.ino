/**
 * RangefinderDemo.ino - Software library for the Make-A-Pede (makeapede.com)
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

// Define Sensor Pins
const uint8_t leftSpeedPin = 5;
const uint8_t leftDirPin = 4;
const uint8_t rightSpeedPin = 6;
const uint8_t rightDirPin = 7;
const uint8_t leftAntennaePin = 12;
const uint8_t rightAntennaePin = 13;

const uint8_t redLEDPin = 9;
const uint8_t greenLEDPin = 10;
const uint8_t blueLEDPin = 11;
const uint8_t rangefinderPin = A1;

// Calibration settings
// Sensor readings for 7, 9, and 11 inches
int a7in = 132; //7 in
int a9in = 102; //9 in
int a11in = 75; //11 in

void setup() {
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(redLEDPin, greenLEDPin, blueLEDPin);
  
  pinMode(rangefinderPin, INPUT);
}

void loop() {
  //Read the input on analog pin 0
  int distance = analogRead(rangefinderPin);
  
  //Print the distance to the serial monitor
  Serial.println(distance);
  
  //Check if the distance reading is less than 7 inches
  if(distance > a7in)
  {setRGBColor(LED_RED);}
  //Check if the distance reading is between 7 and 9 inches
  else if(distance < a7in && distance > a9in)
  {setRGBColor(LED_BLUE);}
  //Check if the distance reading is between 9 and 11 inches
  else if(distance < a9in && distance > a11in)
  {setRGBColor(LED_GREEN);}
  //If neither of the previous two conditions is met, the distance is out of range
  else
  {setRGBColor(LED_OFF);}

  delay(100);    //Wait
}
