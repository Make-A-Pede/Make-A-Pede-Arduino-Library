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
const uint8_t avgCount = 10; // Number of values to use for reading averaging
const uint8_t sensorTiming = 20; // Duration of LED pulses in ms
int redBrightness = 20; // LED pulse brightness
int greenBrightness = 50;
int blueBrightness = 100;
const uint8_t detectionThreshold = 10; // Required difference from other colors to detect as a color
const uint16_t minBrightness = 600; // Brightness range in which to check color
const uint16_t maxBrightness = 800;

// Variables to store measured data
int redVal = 1000;
int greenVal = 1000;
int blueVal = 1000;
int redValArray[avgCount];
int greenValArray[avgCount];
int blueValArray[avgCount];
int selectedColor;

void setup() {
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(redLEDPin, greenLEDPin, blueLEDPin);
  
  pinMode(ldrPin, INPUT_PULLUP); //Enable the internal pullup on the light sensor pin

  redBrightness = 0;
  greenBrightness = 0;
  blueBrightness = 0;
  
  while(redVal > 600){
    // Increase brightness
    redBrightness += 5;
    
    // Measure red value
    setRGBColor(LED_RED, redBrightness);
    delay(sensorTiming);
    redVal = analogRead(ldrPin);

    // Plot on serial plotter
    Serial.print(blueVal);
    Serial.print(",");
    Serial.print(redVal);
    Serial.print(",");
    Serial.print(greenVal);
    Serial.print(",");
    Serial.print(0);
    Serial.print(",");
    Serial.println(0);
  }

  while(greenVal > 600){
    // Increase brightness
    greenBrightness += 5;
    
    // Measure green value
    setRGBColor(LED_GREEN, greenBrightness);
    delay(sensorTiming);
    greenVal = analogRead(ldrPin);

    // Plot on serial plotter
    Serial.print(blueVal);
    Serial.print(",");
    Serial.print(redVal);
    Serial.print(",");
    Serial.print(greenVal);
    Serial.print(",");
    Serial.print(0);
    Serial.print(",");
    Serial.println(0);
  }

  while(blueVal > 600){
    // Increase brightness
    blueBrightness += 5;
    
    // Measure blue value
    setRGBColor(LED_BLUE, blueBrightness);
    delay(sensorTiming);
    blueVal = analogRead(ldrPin);

    // Plot on serial plotter
    Serial.print(blueVal);
    Serial.print(",");
    Serial.print(redVal);
    Serial.print(",");
    Serial.print(greenVal);
    Serial.print(",");
    Serial.print(0);
    Serial.print(",");
    Serial.println(0);
  }
}

void loop() {   
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

  // Compute the average brightness of red, green, and blue
  int avgBrightness = (redVal+greenVal+blueVal)/3;

  // Check if brightness is in the proper range
  if((avgBrightness > minBrightness) && (avgBrightness < maxBrightness)){
    // Determine current color and store
    if((redVal < greenVal-detectionThreshold) && (redVal < blueVal-detectionThreshold)){
        selectedColor = 1; // Red
    } else if((greenVal < redVal-detectionThreshold) && (greenVal < blueVal-detectionThreshold)){
        selectedColor = 2; // Green
    } else if((blueVal < redVal-detectionThreshold) && (blueVal < greenVal-detectionThreshold)){
        selectedColor = 3; // Blue
    } else {
        selectedColor = 0; // Black/White
    }
  } else {
      selectedColor = 0; // Black/White
  }

  // Plot on serial plotter
  Serial.print(blueVal);
  Serial.print(",");
  Serial.print(redVal);
  Serial.print(",");
  Serial.print(greenVal);
  Serial.print(",");
  Serial.print(avgBrightness);
  Serial.print(",");
  Serial.println(selectedColor*100);
}
