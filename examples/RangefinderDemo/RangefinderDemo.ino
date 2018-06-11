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

//#define OBSTACLE_LEFT (getLeftAntennae() == HIGH)
//#define OBSTACLE_RIGHT (getRightAntennae() == HIGH)

// Define Sensor Pins
const uint8_t leftSpeedPin = 5;
const uint8_t leftDirPin = 4;
const uint8_t rightSpeedPin = 6;
const uint8_t rightDirPin = 7;
const uint8_t leftAntennaePin = 12;
const uint8_t rightAntennaePin = 13;

const uint8_t rangefinderPin = A1;

// Calibration settings
// Conversion from sensor reading (x) to distance (y): y = kM * x^kP
// Values found using Excel to record sensor output from serial monitor and plot a trendline
const float kM = 1109.2;
const float kP = -1.061;

// Variable to store sensor reading
float distance;

void setup() {
  // Initialize the serial connection
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  pinMode(rangefinderPin, INPUT);

  // Wait for an antenna to be triggered to start the program
  //Serial.println("Press either antenna to start program");
  //while (!OBSTACLE_LEFT && !OBSTACLE_RIGHT);

  delay(2000);

  distance = convertToInches(analogRead(rangefinderPin));
}

void loop() {
  // Set the motors to run forwards
  setLeftDirection(LOW);
  setRightDirection(LOW);
  
  // Run the motors at 160 power
  setLeftSpeed(160);
  setRightSpeed(160);
  
  // Wait for an obstacle to be < 8" away
  while (distance > 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));
    
    //Print the distance to the serial monitor
    Serial.println(distance);
  }
  
  // Turn left
  setLeftSpeed(0);
  setRightSpeed(160);
     
  // Wait for obstacle to be > 8" away
  int loopCounter = 0;
  while (distance < 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));
    
    //Print the distance to the serial monitor
    Serial.println(distance);

    if(loopCounter > 50) {
      setLeftDirection(HIGH);
      setRightDirection(HIGH);
    }

    loopCounter++;
    delay(10);
  }
  
  // Wait 0.5 seconds
  delay(1000);
}

float convertToInches (int sensorReading) {
  return (kM * (pow(sensorReading, kP)));
}

