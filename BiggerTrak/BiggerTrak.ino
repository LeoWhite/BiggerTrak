#include "IOpins.h"
#include <avr/wdt.h>



typedef struct Motor {
  int power;
  boolean brake;
  int current;
  volatile int encoderCount;
};

Motor motors[2] = {};

// define global variables here
int leftMotorPower = 0, rightMotorPower = 0;           // left and right motor power leves in percentage (-100 to 100)
boolean lmbrake = false, rmbrake = false;              // left and right brakes - non zero values enable brake

unsigned long lastoverload = 0;                            // Time we last overloaded
int overloadtime = 100;

/**
 * Sets up the Arduino ready for use
 */
void setup() {
  // Reset the watchdog in case it triggered
  MCUSR=0;
  wdt_disable();
  
  Serial.begin(115200);         // start serial for output

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
    lastoverload = millis();
    Motors(0, 0);    
  }
  
  // Check if there are any pending i2c commands to process
  I2C_CheckCommands();
    
  // Update any auto drive
  performAutoDrive();
  
  // Reset the watchdog to stop it triggering
  wdt_reset();
}

