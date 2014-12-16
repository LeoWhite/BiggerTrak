// Code for controlling the robot
int targetDistance = 0;
int distanceTravelled = 0;
int powerLeft = 0, powerRight = 0;
boolean autoDriveActive = false;

int oldlmEnc = 0, oldrmEnc = 0;
int kp = 3;
unsigned long autoDriveLastChecked = 0;

/**
 * Stops any current auto drive configuration
 */
void stopAutoDrive() {
  autoDriveActive = false;
}

/**
 * Tell the robot to drive forwards a set distance.
 *
 * @param distance - Distance to drive in millimeters
 * @param power - The power to run the motors at
 */
void driveForwards(int distance, int power) {
  int left, right;
  
  Serial.print("Auto drive, distance is ");
  Serial.print(distance);
  Serial.print(" power is ");
  Serial.println(power);
  
  // Call 'stop' to make sure we are stationary and to reset the encoders
  MotorsStop();
  lmenc = rmenc = 0;
  
  // Configure the target distance and reset
  // the current count
  targetDistance = distance;
  distanceTravelled = 0;
  oldlmEnc = lmenc;
  oldrmEnc = rmenc;
  
  // Start driving
  autoDriveActive = true;
  autoDriveLastChecked = millis();
  lmbrake=false;
  rmbrake=false;
  powerLeft = power;
  powerRight = power;
  Motors(powerLeft, powerRight);
}

/**
 * Tells the robot to rotate the specified number of degrees
 */
void driveRotate(int degreesToTurn) {
  // The number of encoder tickes per degree
  float countsPerDegree = 0.9;
  int left, right;
  
  // Call 'stop' to make sure we are stationary and to reset the encoders
  MotorsStop();

  if(degreesToTurn < 0) {
    left = 60;
    right = -60;
  }
  else {
    left = -60;
    right = 60;
  }
  
  targetDistance = int(abs(countsPerDegree) * degreesToTurn);
  distanceTravelled = 0;
  oldlmEnc = lmenc;
  oldrmEnc = rmenc;
  autoDriveActive = true;
  autoDriveLastChecked = millis();
  lmbrake=false;
  rmbrake=false;
  
  Motors(left, right);    
}

/**
 * Called on each loop to check the status of any in progress
 * auto drive.
 */
void performAutoDrive() {
  // Cache and reset the encoder values immediately
  // as they could change throughout this function.
  int cachedLMEnc = lmenc, cachedRMEnc = rmenc;
  
  // Anything to do?
  if(false == autoDriveActive) {
    return;
  }
  
  // Have we reached (or passed) our destination
  if(targetDistance <= (distanceTravelled + cachedLMEnc)) {
    Serial.print("Auto drive complete> distance travelled:");
    Serial.println(distanceTravelled + cachedLMEnc);
    
    // We've reach out destination
    MotorsStop();
  }
  // Do we need to check the motors for drift?  
  // We don't want to be constantly checking and correcting
  else if((millis() - autoDriveLastChecked) >= 50) {
    int error = (cachedLMEnc - cachedRMEnc) * kp;
    autoDriveLastChecked = millis();
    
    // Adjust the right motor speed to keep up with the
    // left motor
    powerRight += error;
if(powerRight > 100) {
  powerRight = 100;
}
else if(powerRight < 0) {
  powerRight = 0;
}

    // Update the distance travelled and reset the encoder count
    distanceTravelled += lmenc;
    lmenc = rmenc = 0;

    Serial.print("Adjusting for drift ");
    Serial.print((cachedLMEnc - cachedRMEnc));
    Serial.print(" : ");
    Serial.println(error);    

    Motors(powerLeft, powerRight);    
  }
  
  // Have we moved?  
  if(oldlmEnc == cachedLMEnc && oldrmEnc == cachedRMEnc) {
    // We've not moved... so increase power
/*    lmspeed += 1;
    rmspeed += 1;
    lmspeed = min(lmspeed, 255);
    rmspeed = min(rmspeed, 255);
    
    Motors();*/
  }
  else if((targetDistance - distanceTravelled) <= 250) {
    // We are getting closer, so slow down
//    lmspeed = -50;
//    rmspeed = -50;

    
//    Motors();    
  }
    
  // Cache the current encoders  
  oldlmEnc = cachedLMEnc;
  oldrmEnc = cachedRMEnc;  
}


int autoDriveI2C(byte *i2cArgs, uint8_t *pi2cResponse) {
  int distance, power;
  
  // Read in the distance
  distance = i2cArgs[0]*256+i2cArgs[1];
  
  // Read in the power
  power = i2cArgs[2]*256+i2cArgs[3];

  // And lets start driving
  driveForwards(distance, power);

  return 0;
}


int autoDriveI2CRotate(byte *i2cArgs, uint8_t *pi2cResponse) {
  int rotate;
  
  // Read in the distance
  rotate = i2cArgs[0]*256+i2cArgs[1];
  
  // And lets start driving
  driveRotate(rotate);

  return 0;
}



