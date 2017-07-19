# PIR Motion Sensor Example
Sample program for using a PIR motion sensor to control the Make-A-Pede.

## Parts
1 - Assembled Make-A-Pede<br/>
1 - PIR Motion Sensor<br/>
1 - LED<br/>
1 - 1.5k Resistor<br/>
3 - M-F Jumper Wires<br/>

## Wiring
Connect the sensor to your Make-A-Pede using the circuit shown below. The jumper on the back of the sensor should be in the "H" position. The LED is used to show the status of the sensor and is optional.
<br/><br/>
<img src="https://raw.githubusercontent.com/Make-A-Pede/Make-A-Pede-CAD/master/sensor-examples/MaP-PIR-Sensor.png" width="600"/>

## Testing
Plug your Arduino into your computer. The LED connected to the PIR sensor should come on when it detects motion and turn off several seconds after motion is no longer detected. You can use the 2 knobs on the side of the sensor to adjust sensitivity if necessary.

Load PIRDemo.ino onto your Arduino. Turn on the Make-A-Pede and trigger one of the antenna sensors to start the program. Your Make-A-Pede will start driving around when it detects motion. If it stops detecting motion, it will stop driving and wait for motion to be detected again.
