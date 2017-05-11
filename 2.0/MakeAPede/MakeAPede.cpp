/*
 * MakeAPede.cpp - Software library for the Make-A-Pede (makeapede.com)
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

/**
 * Initialize IO for the Make-A-Pede
 * 
 * Must be called in setup before anything other MaP calls
 */
void MakeAPede::start() {
  pinMode(5, OUTPUT);
  //analogWrite(5, 0);

  pinMode(6, OUTPUT);
  //analogWrite(6, 0);

  Serial.begin(9600);
}

void MakeAPede::setLeftMotors(int power) {
  int p = (power/100.0)*255;
  analogWrite(6, p);
}

void MakeAPede::setRightMotors(int power) {
  int p = (power/100.0)*255;
  analogWrite(5, p);
}

/*
 * Waits for the specified time in millis while periodically calling backgroundTasks
 */
void MakeAPede::wait(unsigned long ms) {
  unsigned long t = millis();

  while(millis()-t < ms) {
    backgroundTasks();
    delayMicroseconds(200);
  }
}

/**
 * Recieves bluetooth messages from the app, interprets them, and sets motors to the appropriate values
 * 
 * Must be called frequently for smooth control
 */
void MakeAPede::btControl() {
  if(btEnabled) {
    if(Serial.available() > 0) {
      char command[9];
      Serial.readBytes(command, 9);
      Serial.print(command);
      
      char* p;
      p = strtok(command, ":");
      int left = atoi(p);
  
      p = strtok(NULL, ":");
      int right = atoi(p);

      p = strtok(NULL, ":");
      int speaker = atoi(p);
  
      setLeftMotors(left);
      setRightMotors(right);     
  
      btTimeOfLastMessage = millis();
    } else if(millis() - btTimeOfLastMessage > 5000) {
      setLeftMotors(0);
      setRightMotors(0);
    }
  }
}

/**
 * Sets whether or not the bluetooth is enabled
 */
void MakeAPede::setBTEnabled(bool b) {
  btEnabled = b;
}

/**
 * Calls updateMotors and btControl. Should be called every ~250 micros.
 */
void MakeAPede::backgroundTasks() {
  btControl();
}

