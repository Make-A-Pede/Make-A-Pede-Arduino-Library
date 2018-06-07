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
const uint8_t redBrightness = 20; // LED pulse brightness
const uint8_t greenBrightness = 50;
const uint8_t blueBrightness = 100;
const uint8_t detectionThreshold = 10; // Required difference from other colors to detect as a color
const uint16_t minBrightness = 400; // Brightness range in which to check color
const uint16_t maxBrightness = 800;

// Variables to store measured data
int redVal;
int greenVal;
int blueVal;
int avgBrightness;
int redValArray[avgCount];
int greenValArray[avgCount];
int blueValArray[avgCount];
int avgBrightnessArray[avgCount];
int selectedColor;

void setup() {
  Serial.begin(9600);

  setupMaP(leftSpeedPin, leftDirPin, rightSpeedPin, rightDirPin, leftAntennaePin, rightAntennaePin);
  setupRGB(redLEDPin, greenLEDPin, blueLEDPin);
  
  pinMode(ldrPin, INPUT_PULLUP); //Enable the internal pullup on the light sensor pin
}

void loop() {   
  /*
   * Read Sensor
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

  // Compute the average brightness of red, green, and blue
  avgBrightness = (redVal+greenVal+blueVal)/3;

  /*
   * Filter Data
   */
  // Adjust red, green, and blue values based on average brightness
  int brightnessShift = avgBrightness - 500;
  redVal = redVal - brightnessShift;
  greenVal = greenVal - brightnessShift;
  blueVal = blueVal - brightnessShift;

  // Average last 10 values
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

  redVal = redValSum/avgCount;
  greenVal = greenValSum/avgCount;
  blueVal = blueValSum/avgCount;
  avgBrightness = avgValSum/avgCount;
  
  /*
   * Determine Result
   */
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

  /*
   * Plot
   */
  // Plot on serial plotter
  Serial.print(blueVal); // Blue line - blue value
  Serial.print(",");
  Serial.print(redVal); // Red line - red value
  Serial.print(",");
  Serial.print(greenVal); // Green line - green value
  Serial.print(",");
  Serial.print(avgBrightness); // Orange line - brightness
  Serial.print(",");
  Serial.println(selectedColor*100); // Purple line - selected color
}
