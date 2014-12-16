#include "IOpins.h"
#include <avr/wdt.h>




// define global variables here
int  lowbat=550;                                       // default low battery voltage is 5.5V
int lmspeed,rmspeed;                                   // left and right motor speeds -255 to +255
boolean lmbrake = false,rmbrake = false;                                  // left and right brakes - non zero values enable brake
int lmcur,rmcur;                                       // left and right motor current

int lmcurmax = 8000;                                   // Max current that can be pulled from the batteries before we stop

unsigned long lastoverload = 0;                            // Time we last overloaded
int overloadtime = 100;

volatile int  lmenc = 0,rmenc = 0;                                       // left and right encoder values

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
  wdt_enable(WDTO_500MS);
  
  Serial.println("Setup complete");
}

/**
 * main loop 
 */
void loop() {
  // Read in the current amps
  lmcur=(analogRead(lmcurpin)-511)*48.83;
  rmcur=(analogRead(rmcurpin)-511)*48.83;  
  
  // Check if we've gone over the limit
  if(lmcur >= lmcurmax || rmcur >= lmcurmax) {
    // Mark the fact that we have overloaded and trigger
    // an update of the Motors, this will cause them to stop
    lastoverload = millis();
    Motors(0, 0);    
  }
  
  // Check if there are any pendign i2c commands to process
  I2C_CheckCommands();
    
  // Update any auto drive
  performAutoDrive();
  
  // Reset the watchdog to stop it triggering
  wdt_reset();
}



