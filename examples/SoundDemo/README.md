# Sound Sensor Example
Sample program for using a sound sensor to control the Make-A-Pede.

## Parts
1 - Assembled Make-A-Pede<br/>
1 - Electret Mic Breakout Board<br/>
3 - M-F Jumper Wires<br/>

## Wiring
Connect the sensor to your Make-A-Pede using the circuit shown below. Be sure to check the pin labels on your specific breakout board. If your sensor has both analog and digital outputs, you only need to connect the analog output.
<br/><br/>
<img src="https://raw.githubusercontent.com/Make-A-Pede/Make-A-Pede-CAD/master/sensor-examples/MaP-Sound-Sensor.png" width="600"/>

## Testing
Load SoundDemo.ino onto your Arduino. Turn on the Make-A-Pede and trigger one of the antenna sensors to calibrate the sensor and start the program. When it detects a loud sound, your Make-A-Pede will drive around until it had bumped into 3 objects. It will then wait for you to trigger it again with another sound. If it does not stop driving to wait for another sound, try re-calibrating the sensor by pressing the reset button to restart the program.