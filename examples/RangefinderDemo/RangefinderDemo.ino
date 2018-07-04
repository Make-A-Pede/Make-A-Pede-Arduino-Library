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
const uint8_t avgCount = 10; // Number of values to use for averaging
// Conversion from IR sensor reading (x) to distance (y): y = kM * x^kP
// Values found using Excel to record sensor output from serial monitor and plot a trendline
const float kM = 1109.2;
const float kP = -1.061;

// Variables to store motor settings
const int motorSpeed = 175;

// Variable to store sensor readings
float distance;
float distanceArray[avgCount];

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
  
  // Run the motors forward
  setLeftSpeed(motorSpeed+15);
  setRightSpeed(motorSpeed-15);
  
  // Wait for an obstacle to be < 8" away
  while (distance > 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));
    
    //Print the distance to the serial monitor
    Serial.println(distance);
  }
  
  // Turn left
  setLeftSpeed(0);
  setRightSpeed(motorSpeed);
     
  // Wait for obstacle to be > 8" away
  int loopCounter = 0;
  while (distance < 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));
    
    //Print the distance to the serial monitor
    Serial.println(distance);

    // If object is too close or timeout has expired
    if((loopCounter > 1000) || (distance < 2)) {
      // Back up
      setLeftSpeed(motorSpeed);
      setRightSpeed(motorSpeed);
      setLeftDirection(HIGH);
      setRightDirection(HIGH);

      delay(750);
      break;
    }

    loopCounter++;
    delay(10);
  }
}

/*
 * Convert sensor reading to inches and filter
 */
float convertToInches (int sensorReading) {
  float result = kM * (pow(sensorReading, kP));

  /*if (result > 12) {
    result = 12;
  }*/
  
  // Average last values
  int i;
  for (i = 0; i < (avgCount-1); i++) {
    distanceArray[i] = distanceArray[i+1];
  }

  distanceArray[avgCount-1] = result;

  result = 0;
  for (i = 0; i < avgCount; i++) {
    result += distanceArray[i];
  }

  result = result/avgCount;
  
  return result;
}

