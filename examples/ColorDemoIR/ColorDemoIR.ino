/**
 * ColorDemoIR.ino - Software library for the Make-A-Pede (makeapede.com)
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

const uint8_t redLEDPin = 9;
const uint8_t greenLEDPin = 10;
const uint8_t blueLEDPin = 11;
const uint8_t ldrPin = A0;

const uint8_t rangefinderPin = A1;

// Calibration settings
const uint8_t avgCount = 5; // Number of values to use for averaging
const uint8_t sensorTiming = 20; // Duration of LED pulses in ms
const int whiteThreshold = 500; // Target brightness for automatic calibration
const int blackThreshold = 200; // Brightness cutoff for checking color
const uint8_t detectionThreshold = 80; // Threshold to select color
int redBrightness; // LED pulse brightness (will be automatically adjusted)
int greenBrightness;
int blueBrightness;
// Conversion from sensor reading (x) to distance (y): y = kM * x^kP
// Values found using Excel to record sensor output from serial monitor and plot a trendline
const float kM = 1109.2;
const float kP = -1.061;

// Variables to store measured color data
int redVal;
int greenVal;
int blueVal;
int avgBrightness;
int redValArray[avgCount];
int greenValArray[avgCount];
int blueValArray[avgCount];
int avgBrightnessArray[avgCount];

// Variables to store calculated color values
int yellowVal;

// Variable to store IR sensor readings
float distance;
float distanceArray[avgCount];

// Variables to store motor settings
const int defaultSpeed = 150;
int motorSpeed = defaultSpeed;

void setup() {
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(redLEDPin, greenLEDPin, blueLEDPin);
  
  pinMode(ldrPin, INPUT_PULLUP); //Enable the internal pullup on the light sensor pin
  pinMode(rangefinderPin, INPUT);

  /*
   * Sensor Calibration
   */
  redBrightness = 0;
  greenBrightness = 0;
  blueBrightness = 0;
  
  while(redVal < whiteThreshold){
    // Increase brightness
    redBrightness += 5;
    
    // Measure red value
    setRGBColor(LED_RED, redBrightness);
    delay(sensorTiming);
    redVal = 1024 - analogRead(ldrPin);
  }
  setRGBColor(LED_OFF);
  delay(sensorTiming);

  while(greenVal < whiteThreshold){
    // Increase brightness
    greenBrightness += 5;
    
    // Measure green value
    setRGBColor(LED_GREEN, greenBrightness);
    delay(sensorTiming);
    greenVal = 1024 - analogRead(ldrPin);
  }
  setRGBColor(LED_OFF);
  delay(sensorTiming);

  while(blueVal < whiteThreshold){
    // Increase brightness
    blueBrightness += 5;
    
    // Measure blue value
    setRGBColor(LED_BLUE, blueBrightness);
    delay(sensorTiming);
    blueVal = 1024 - analogRead(ldrPin);
  }
  setRGBColor(LED_OFF);
  delay(sensorTiming);

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
  
  // Wait for an obstacle to be < 8" away
  while (distance > 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));

    switch(getColor()){
      case 0: // Black
        // motorSpeed = defaultSpeed;
        break;
      case 1: // Red
        motorSpeed = 0;
        break;
      case 2: // Green
        motorSpeed = defaultSpeed + 100;
        break;
      case 3: // Blue
        break;
      case 4: // Yellow
        break;
    }

    Serial.println(motorSpeed);

    // Run the motors
    setLeftSpeed(motorSpeed+15);
    setRightSpeed(motorSpeed-15);
  }

  motorSpeed = defaultSpeed;
  
  // Turn left
  setLeftSpeed(0);
  setRightSpeed(motorSpeed);
     
  // Wait for obstacle to be > 8" away
  int loopCounter = 0;
  while (distance < 8){
    //Read the sensor and convert to inches
    distance = convertToInches(analogRead(rangefinderPin));

    // If object is too close or timeout has expired
    if((loopCounter > 1000) || (distance < 2)) {
      // Back up
      setLeftSpeed(motorSpeed);
      setRightSpeed(motorSpeed);
      setLeftDirection(HIGH);
      setRightDirection(HIGH);
    } else {
      
    }

    loopCounter++;
    delay(10);
  }
}

/*
 * Read Color Sensor
 */
int getColor(){
  // Measure red value
  setRGBColor(LED_RED, redBrightness);
  delay(sensorTiming);
  redVal = analogRead(ldrPin);
  setRGBColor(LED_OFF);
  delay(sensorTiming);

  // Measure green value
  setRGBColor(LED_GREEN, greenBrightness);
  delay(sensorTiming);
  greenVal = analogRead(ldrPin);
  setRGBColor(LED_OFF);
  delay(sensorTiming);

  // Measure blue value
  setRGBColor(LED_BLUE, blueBrightness);
  delay(sensorTiming);
  blueVal = analogRead(ldrPin);
  setRGBColor(LED_OFF);
  delay(sensorTiming);

  // Compute the average brightness of red, green, and blue
  avgBrightness = (redVal+greenVal+blueVal)/3;

  /*
   * Filter Data
   */
  // Average last values
  int i;
  for (i = 0; i < (avgCount-1); i++) {
    redValArray[i] = redValArray[i+1];
    greenValArray[i] = greenValArray[i+1];
    blueValArray[i] = blueValArray[i+1];
    avgBrightnessArray[i] = avgBrightnessArray[i+1];
  }

  redValArray[avgCount-1] = redVal;
  greenValArray[avgCount-1] = greenVal;
  blueValArray[avgCount-1] = blueVal;
  avgBrightnessArray[avgCount-1] = avgBrightness;

  int redValSum = 0;
  int greenValSum = 0;
  int blueValSum = 0;
  int avgValSum = 0;

  for (i = 0; i < avgCount; i++) {
    redValSum += redValArray[i];
    greenValSum += greenValArray[i];
    blueValSum += blueValArray[i];
    avgValSum += avgBrightnessArray[i];
  }

  redVal = 1024-(redValSum/avgCount);
  greenVal = 1024-(greenValSum/avgCount);
  blueVal = 1024-(blueValSum/avgCount);
  avgBrightness = 1024-(avgValSum/avgCount);

  // Shift values based on average brightness
  redVal = redVal - avgBrightness;
  greenVal = greenVal - avgBrightness;
  blueVal = blueVal - avgBrightness;
  
  // Scale values based on average peak deviation
  float scale = 100.0/((abs(redVal) + abs(blueVal) + abs(greenVal))/3);
  redVal = redVal*scale;
  greenVal = greenVal*scale;
  blueVal = blueVal*scale;

  // Compute additional colors
  yellowVal = (redVal + greenVal)/1.5;

  /*
   * Determine color
   */
   int detectedColor = 0;
   
   if(avgBrightness < blackThreshold){
    detectedColor = 0;
   }
   else if(yellowVal > detectionThreshold){
    detectedColor = 4;
   }
   else if(redVal > detectionThreshold){
    detectedColor = 1;
   }
   else if(blueVal > detectionThreshold){
    detectedColor = 3;
   }
   else if(greenVal > detectionThreshold){
    detectedColor = 2;
   }
  
  /*
   * Plot
   */
  // Plot on serial plotter
  /*Serial.print(blueVal); // Blue
  Serial.print(",");
  Serial.print(redVal); // Red
  Serial.print(",");
  Serial.print(greenVal); // Green
  Serial.print(",");
  Serial.print(yellowVal); // Orange
  Serial.print(",");
  Serial.print(avgBrightness); // Purple
  Serial.print(",");
  Serial.println(detectedColor * 20); // Grey*/

  return detectedColor;
}

/*
 * Convert IR sensor reading to inches and filter
 */
float convertToInches (int sensorReading) {
  float result = kM * (pow(sensorReading, kP));
  
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

