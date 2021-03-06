
// How long to disable the motors after an overload
// has been 
#define OVERLOAD_COOLDOWN_MS 100

// What is the voltage we want to run the motors at
#define MOTOR_TARGET_VOLTAGE 600

int powerRatio = 0;

/**
 * Configures the Arduino's pins that will be used to control the motors
 */
void motorsSetup() {
  int batteryVoltage;

  TCCR2B = TCCR2B & B11111000 | B00000110;   // set timer 2 divisor to  256 for PWM frequency of    122.070312500 Hz
  
  // Setup the left motor
  pinMode(lmpwmpin,OUTPUT);
  pinMode(lmdirpin,OUTPUT);
  pinMode(lmbrkpin,OUTPUT);
  
  // Setup the right motor
  pinMode(rmpwmpin,OUTPUT);
  pinMode(rmdirpin,OUTPUT);
  pinMode(rmbrkpin,OUTPUT);
  
  // Initialise the motor arrays
  memset(motors, 0, sizeof(motors));
  
  // Work out how much power we should be applying to the
  // motors to reach the target voltage
  batteryVoltage = analogRead(voltspin)*10/3.357; 
  
  powerRatio = (int)(((float)MOTOR_TARGET_VOLTAGE / (float)batteryVoltage) * 100.0);

  // Ensure the motors are not running
  MotorsStop();
}

/** 
 * Sets the speed of the motors.
 * The motor power is convereted into a percentage so this
 * routine takes in values from -100 to 100, with the sign of
 * indicating the direction (i.e. negative numbers will
 * make the motor go backwards)
 *
 * @param left - Speed of the left motor
 * @param right - Speed of the right motor 
 */
void Motors(int left, int right)
{ 
  int lmspeed,rmspeed;
  
  // Are we in an 'overload' state?
  if((millis() - lastOverloadMS) < OVERLOAD_COOLDOWN_MS) {
    // Set speed to zero and brakes to on
    lmspeed = 0;
    motors[LEFT_MOTOR].brake=true;
    rmspeed = 0;
    motors[RIGHT_MOTOR].brake=true;
  }  
  
  // Are the inputs valid
  if(left > 100 || left < -100 || right > 100 || right < -100) {
    Serial.println("Motor input invalid, ignoring");
    return;
  }

  // Check for stopping
/*  if(lmspeed == 0) {
    lmbrake = true;
    lmspeed = 100;
  }
  else {
    lmbrake = false;
  }
  
  if(rmspeed == 0) {
    rmbrake = true;
    rmspeed = 100;
  }
  else {
    rmbrake = false;
  }
  */

  // Store the percentage values
  motors[LEFT_MOTOR].power = left;
  motors[RIGHT_MOTOR].power = right;
  
  // Convert from percentage to actual value
  lmspeed = (abs(motors[LEFT_MOTOR].power) * 0xFF) / 100;
  rmspeed = (abs(motors[RIGHT_MOTOR].power) * 0xFF) /100;
  
  // Convert to take into account the difference between
  // the input voltage and the target output voltage
  lmspeed = (lmspeed * powerRatio) / 100;
  rmspeed = (rmspeed * powerRatio) / 100;
  
  digitalWrite(lmbrkpin,motors[LEFT_MOTOR].brake);                     // if left brake>0 then engage electronic braking for left motor
  digitalWrite(lmdirpin,motors[LEFT_MOTOR].power > 0);                     // if left speed>0 then left motor direction is forward else reverse
  analogWrite (lmpwmpin,lmspeed);                  // set left PWM to absolute value of left speed - if brake is engaged then PWM controls braking
  if(motors[LEFT_MOTOR].brake>0 && motors[LEFT_MOTOR].power==0) {
    motors[LEFT_MOTOR].encoderCount=0;                  // if left brake is enabled and left speed=0 then reset left encoder counter
  }
  
  digitalWrite(rmbrkpin,motors[RIGHT_MOTOR].brake);                     // if right brake>0 then engage electronic braking for right motor#
  digitalWrite(rmdirpin,motors[RIGHT_MOTOR].power > 0);                     // if right speed>0 then right motor direction is forward else reverse
  analogWrite (rmpwmpin,rmspeed);                  // set right PWM to absolute value of right speed - if brake is engaged then PWM controls braking
  if(motors[RIGHT_MOTOR].brake>0 && motors[RIGHT_MOTOR].power==0) {
    motors[RIGHT_MOTOR].encoderCount=0;                  // if right brake is enabled and right speed=0 then reset right encoder counter
  }
  
  // Update the LEDS
  rearLightUpdate();
  

Serial.print("Motors =");
  Serial.print(motors[LEFT_MOTOR].power);
  Serial.print(":");
  Serial.println(motors[RIGHT_MOTOR].power);
  
}

/**
 * Stops the motors and enables the brakes
 */
void MotorsStop() {
  // Enable brakes
  motors[LEFT_MOTOR].brake=true;
  motors[RIGHT_MOTOR].brake=true;
  
  // Stopping the motors also cancels auto drive
  stopAutoDrive();
  Motors(0, 0);
}

/**
 * Process the i2c stop command
 */
int motorsI2CStop(byte *i2cArgs, uint8_t *pi2cResponse) {
  // No arguments to process, so just stop the motors
  MotorsStop();
  
  return 0;
}

/**
 * Process the i2c set motors command
 *
 * Takes in the two integers that contain the power levels
 * of the left and right motors
 */
int motorsI2CSet(byte *i2cArgs, uint8_t *pi2cResponse) {
  int left, right;
  boolean gotLeft = false, gotRight = false;

  // read integer from I²C buffer
  left=i2cArgs[0]*256+i2cArgs[1];                                               
  if(left >= -100 && left <= 100)
  { 
    motors[LEFT_MOTOR].brake=false;
    gotLeft = true;
  }

  // read integer from I²C buffer
  right=i2cArgs[2]*256+i2cArgs[3];
  if(right >= -100 && right <= 100)
  {
    motors[RIGHT_MOTOR].brake=false;
    gotRight = true;
  }

  if(gotLeft && gotRight) {      
    // The user is now in control, so disable auto drive
    stopAutoDrive();
    
    Motors(left, right);
  }
    
  return 0;
}
