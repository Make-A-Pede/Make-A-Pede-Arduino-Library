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

#define OBSTACLE_LEFT (getLeftAntennae() == HIGH)
#define OBSTACLE_RIGHT (getRightAntennae() == HIGH)

void setup() {
  setupMaP();

  enableObstacleAvoid(true);

  while (!OBSTACLE_LEFT && !OBSTACLE_RIGHT);

  delay(2000);

  while (true) {
    setLeftDirection(LOW);
    setRightDirection(LOW);

    setLeftSpeed(160);
    setRightSpeed(160);

    while (!OBSTACLE_LEFT && !OBSTACLE_RIGHT);

    setLeftSpeed(0);
    setRightSpeed(0);

    delay(250);

    setLeftDirection(HIGH);
    setRightDirection(HIGH);

    if (OBSTACLE_LEFT) {
      setLeftSpeed(90);
      setRightSpeed(200);
    } else if (OBSTACLE_RIGHT) {
      setLeftSpeed(200);
      setRightSpeed(90);
    }

    while (OBSTACLE_LEFT || OBSTACLE_RIGHT);

    delay(1500);

    setLeftSpeed(0);
    setRightSpeed(0);

    delay(250);
  }
}

void loop() {}

void userCode() {}
