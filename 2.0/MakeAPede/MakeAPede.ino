/**
 * MakeAPede.ino - Software library for the Make-A-Pede (makeapede.com)
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

MakeAPede MaP;

void setup() {
  //pinMode(5, OUTPUT);
  //pinMode(6, OUTPUT);

  MaP.start();

  Serial.println("Setup");

  //MaP.setBTEnabled(true);
}

void loop() {
  //MaP.backgroundTasks();

  Serial.println("Up!");
  for(int i = 0; i <= 100; i++) {
    MaP.setLeftMotors(i);
    MaP.setRightMotors(i);

    delay(15);
  }

  delay(1000);
  
  Serial.println("Down!");
  for(int i = 100; i >= 0; i--) {
    MaP.setLeftMotors(i);
    MaP.setRightMotors(i);

    delay(15);
  }

  delay(1000);
 
  //analogWrite(5, 0);
  //analogWrite(6, 0);

  //delayMicroseconds(250);
}

