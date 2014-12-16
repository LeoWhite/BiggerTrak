
//------------------------- Define function of each I/O pin -------------------------------------------------

#define lmencpin     6  //  D6 - left  motor encoder input - optional
#define rmencpin     5  //  D5 - right motor encoder input - optional

#define lmenc_secondpin     13  //  D6 - left  motor encoder input - optional
#define rmenc_secondpin     12  //  D5 - right motor encoder input - optional


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

//------------------------- Servo Support -------------------------------------------------------------------
// Servos are defined as required so that the digital pins may be used for other functions
// The sample code defines servo positions using the writeMicrosecond() command for greater precision 1500=center
// A servo position of 0 indicates that a servo is not used and the pin is free for other purposes
// When required - servos 0-5 use I/O pins D7, D8, D12, D13, D5, D6
// Servo headers for D7, D8, D12, D13 are powered from +6V or +Bat when in low voltage mode.
// Servo headers for D5 and D6 are powered from +5V and are best suited for use with low power servos e.g. pan and tilt

//------------------------- alternate definitions for RC mode -----------------------------------------------

#define RCsteerpin   6  //  D6 - RC steering input        only valid in mode 1
#define RCspeedpin   5  //  D5 - RC throttle input        only valid in mode 1

#define RearLightPin 7 // Read NeoPixel light

#define ServoMax 1
#define ServoOnePin 8 // Front mounted Servo

