#include "BiggerTrak.h"
#include <avr/wdt.h>


// Define a structure to hold information about the motors
typedef struct Motor {
  int power; // Current power level of the motor from -100 to 100
  boolean brake; // If true then enable the electric brake
  int current;   // Current being pulled by the motor in milli-amps
  volatile int encoderCount; // How far this motor has turned since the count was last reset
};


// Array to hold details on the 2 motors this system has
Motor motors[2];

// Time we last detected an overloaded
unsigned long lastOverloadMS = 0; 

/**
 * Sets up the Arduino ready for use
 */
void setup() {
  // Reset the watchdog in case it triggered
  MCUSR=0;
  wdt_disable();
  
  // start serial for output (Used for debugging)
  Serial.begin(115200);    

  // Configure motor
  motorsSetup();

  // Configure motor encoders
  encodersSetup();
  
  // initialize i2c
  I2CSetup();
  
  // Initialise the Servos
  servoSetup();
  
  // Start the NeoPixel
  rearLightSetup();
  
  // With setup complete we now enable the watchdog
  wdt_enable(WDTO_250MS);
  
  Serial.println("Setup complete");
}

/**
 * main loop 
 */
void loop() {
  // Read in the current amps
  motors[LEFT_MOTOR].current=(analogRead(lmcurpin)-511)*48.83;
  motors[RIGHT_MOTOR].current=(analogRead(rmcurpin)-511)*48.83;  
  
  // Check if we've gone over the limit
  if(motors[LEFT_MOTOR].current >= CURRENT_OVERLOAD_CUTOUT || motors[RIGHT_MOTOR].current >= CURRENT_OVERLOAD_CUTOUT) {
    // Mark the fact that we have overloaded and trigger
    // an update of the Motors, this will cause them to stop
    lastOverloadMS = millis();
    Motors(0, 0);    
  }
  
  // Check if there are any pending i2c commands to process
  I2C_CheckCommands();
    
  // Update any auto drive
  performAutoDrive();
  
  // Reset the watchdog to stop it triggering
  wdt_reset();
}

