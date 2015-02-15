
//------------------------- Define function of each I/O pin -------------------------------------------------

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

#define CURRENT_OVERLOAD_CUTOUT 20000

#define lmencpin     6  //  D6 - left  motor encoder input
#define lmenc_secondpin     8  // D8 - left  motor encoder second pin

#define rmencpin     5  //  D5 - right motor encoder input
#define rmenc_secondpin     7  //  D7 - right motor encoder second pin


#define lmbrkpin     4  //  D4 - left  motor brake        control    pin    HIGH = Brake 
#define lmdirpin     2  //  D2 - left  motor direction    control    pin    HIGH = Forward   Low = Reverse
#define lmpwmpin     3  //  D3 - left  motor pulse width  modulation pin    0 - 255          Speed and Brake 
#define lmcurpin     7  //  A6 - left  motor current      monitor    pin    0 - 1023         -20A to +20A   

#define rmbrkpin     9  //  D9 - right motor brake        control    pin    HIGH = Brake 
#define rmdirpin    10  // D10 - right motor direction    control    pin    HIGH = Forward   Low = Reverse
#define rmpwmpin    11  // D11 - right motor pulse width  modulation pin    0 - 255          Speed and Brake 
#define rmcurpin     6  //  A7 - right motor current      monitor    pin    0 - 1023         -20A to +20A   

#define voltspin     3  //  A3 - battery voltage          1V = 33.57        30V = 1007

#define axisxpin     0  //  A0 - accelerometer X-axis
#define axisypin     1  //  A1 - accelerometer Y-axis
#define axiszpin     2  //  A2 - accelerometer Z-axis


#define RearLightPin 13 // D13 - Read NeoPixel light

#define ServoMax 1
#define ServoOnePin 12 // D12 - Front mounted Servo - Optional

typedef struct Motor {
  int power;
  boolean brake;
  int current;
  volatile int encoderCount;
};

