/*
 * MakeAPede.h - Software library for the Make-A-Pede (makeapede.com)
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

#ifndef MakeAPede_h
#define MakeAPede_h

#include "Arduino.h"

class MakeAPede {
  public:
    void start();

    void setLeftMotors(int power);
    void setRightMotors(int power);
    
    void wait(unsigned long ms);

    void btControl();
    void btSend(int data);
    void setBTEnabled(bool b);

    void backgroundTasks();

  private:
    unsigned long btTimeOfLastMessage = millis();

    bool btEnabled = false;
};

#endif

