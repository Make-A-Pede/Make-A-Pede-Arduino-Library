/**
 * ColorDemo.ino - Software library for the Make-A-Pede (makeapede.com)
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
const uint8_t ldrPin = A0;

// Calibration settings
const uint8_t avgCount = 10; // Duration of LED pulses in ms
const uint8_t sensorTiming = 20; // Duration of LED pulses in ms
int redBrightness = 20;
int greenBrightness = 50;
int blueBrightness = 100;

// Variables to store measured brightness
int redValArray[avgCount];
int greenValArray[avgCount];
int blueValArray[avgCount];
int redVal;
int greenVal;
int blueVal;

// Variables for recieving calibration messages
int currentColor = 0;
char* currentAction = "0";
char* currentValue = "000";

void setup() {
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(redLEDPin, greenLEDPin, blueLEDPin);
  
  pinMode(ldrPin, INPUT_PULLUP); //Enable the internal pullup on the light sensor pin
}

void loop() {
  /*
   * Color Sensing
   * Cycles through red, green, and blue LED settings and compares the amount of light reflected in each case.
   */
   
  // Measure red value
  setRGBColor(LED_RED, redBrightness);
  delay(sensorTiming);
  redVal = analogRead(ldrPin);

  // Measure green value
  setRGBColor(LED_GREEN, greenBrightness);
  delay(sensorTiming);
  greenVal = analogRead(ldrPin);

  // Measure blue value
  setRGBColor(LED_BLUE, blueBrightness);
  delay(sensorTiming);
  blueVal = analogRead(ldrPin);

  // Average last 10 values
  int i;
  for (i = 0; i < (avgCount-1); i++) {
    redValArray[i] = redValArray[i+1];
    greenValArray[i] = greenValArray[i+1];
    blueValArray[i] = blueValArray[i+1];
  }

  redValArray[avgCount-1] = redVal;
  greenValArray[avgCount-1] = greenVal;
  blueValArray[avgCount-1] = blueVal;

  int redValSum = 0;
  int greenValSum = 0;
  int blueValSum = 0;

  for (i = 0; i < avgCount; i++) {
    redValSum += redValArray[i];
    greenValSum += greenValArray[i];
    blueValSum += blueValArray[i];
  }

  redVal = redValSum/avgCount;
  greenVal = greenValSum/avgCount;
  blueVal = blueValSum/avgCount;

  // Print measured values
  /*Serial.print("R:");
  Serial.print(redVal);
  Serial.print(", G:");
  Serial.print(greenVal);
  Serial.print(", B:");
  Serial.print(blueVal);
  Serial.print(" | ");*/

  // Print current calibration settings
  /*Serial.print("RB:");
  Serial.print(redBrightness);
  Serial.print(", GB:");
  Serial.print(greenBrightness);
  Serial.print(", BB:");
  Serial.print(blueBrightness);
  Serial.print(" | ");*/

  // Determine current color and display
  /*if((redVal < greenVal) && (redVal < blueVal)){
      Serial.println("Red");
  } else if((greenVal < redVal) && (greenVal < blueVal)){
      Serial.println("Green");
  } else {
      Serial.println("Blue");
  }*/

  // Plot on serial plotter
  Serial.print(blueVal);
  Serial.print(",");
  Serial.print(redVal);
  Serial.print(",");
  Serial.println(greenVal);
  

  /*
   *  Calibration
   *  Reads incoming serial messages and uses them to set calibration values.  Final values will need to be manually entered at the top of the program.
   */
   
  // Check if a calibration message has been recieved
  // Message format is color:brightness:
  // For example: r:85:
  if (Serial.available() > 0) {
    // If there is a message, read it into a char array and print it
    char msg[7];
    Serial.readBytes(msg, 7);
    Serial.println(msg);

    // Separate the color value from the message
    currentAction = strtok(msg, ":");
    currentColor = currentAction[0];
    
    // Separate the brightness value from the message
    currentValue = strtok(NULL, ":");

    // Modify the calibration setting for the recieved color to match the received value
    switch(currentColor) {
      case 'r': // Red
        redBrightness = atoi(currentValue);
        break;
        
      case 'g': // Green
        greenBrightness = atoi(currentValue);
        break;

      case 'b': // Blue
        blueBrightness = atoi(currentValue);
        break;
    }
  }
}
