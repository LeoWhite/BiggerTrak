#include <Servo.h>  

int servoPos = 0;
Servo servoOne;

#define SERVO_MIN_POSITION 800
#define SERVO_MAX_POSITION 2500
#define SERVO_MID_POSITION (((SERVO_MAX_POSITION - SERVO_MIN_POSITION) / 2) + SERVO_MIN_POSITION)


void servoSetup() {
  // Setup servo. Currently we only support the one!
  servoOne.attach(ServoOnePin);
  
  // Set to its default position
  servoOne.writeMicroseconds(SERVO_MAX_POSITION);
  
}

void servoSetPosition(int newPosition) {
  // Double check the limits
  if(newPosition < SERVO_MIN_POSITION) {
    newPosition = SERVO_MIN_POSITION;
  }
  else if(newPosition > SERVO_MAX_POSITION) {
    newPosition = SERVO_MAX_POSITION;
  }
  
  // Update the position of the servo
  servoOne.writeMicroseconds(newPosition);  
}

/**
 * Process the i2c set servo command
 *
 * Takes in an integer that defines the position of the servo
 */
int servoI2CSet(byte *i2cArgs, uint8_t *pi2cResponse) {
  int servoPosition;
  boolean gotPosition = false;

  // read integer from I²C buffer
  servoPosition=i2cArgs[0]*256+i2cArgs[1];                                               
  if(servoPosition >= SERVO_MIN_POSITION && servoPosition <= SERVO_MAX_POSITION)
  { 
    gotPosition=true;
  }

  if(gotPosition) {
    servoSetPosition(servoPosition);
  }
    
  return 0;
}
