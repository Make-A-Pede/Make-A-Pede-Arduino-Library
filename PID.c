#define Kp 0.0166666
#define Kd 0.357
float error = 0;
float previouserror = 0;
float derivative = 0;
float PDo (float heading, float target){
  error = target - heading;
  derivative = error - previouserror;
  return (int) (Kp*error + Kd*derivative);

}
