#include <Adafruit_NeoPixel.h>


// Light strip for the rear light
Adafruit_NeoPixel rearNeoStrip = Adafruit_NeoPixel(8, RearLightPin, NEO_GRB + NEO_KHZ800);

/**
 * Configures the rear NeoPixel strip ready for use,
 * setting up the brightness and turns on the end lights
 * to indicate its loaded.
 */
void rearLightSetup() {
  rearNeoStrip.begin();
  rearNeoStrip.setBrightness(0x7f);
  rearNeoStrip.show();
  
  rearNeoStrip.setPixelColor(0, rearNeoStrip.Color(255, 0, 0)); 
  rearNeoStrip.setPixelColor(7, rearNeoStrip.Color(255, 0, 0)); 
  rearNeoStrip.show();
}

/**
 * Updates the pattern of lights on the rear NeoPixel strip
 * to reflect the current power usage of the motors
 */
void rearLightUpdate() {
  for(uint8_t i = 0; i < 2; i++)
  {
    uint8_t LED0 = 0, LED1 = 0, LED2 = 0, LED3 = 0;
    uint8_t power = 0, dir = 0;
    uint32_t colour0, colour1, colour2, colour3;
    
    if(i == 0) {
      power = abs(lmspeed);
      dir = lmspeed > 0;
    }
    else {
      power = abs(rmspeed);
      dir = rmspeed > 0;
    }
    
    if(power == 0xFF) {
      LED0 = 0xFF;
      LED1 = 0xFF;
      LED2 = 0xFF;
      LED3 = 0xFF;
    }
    else if(power > 0xBF) {
      LED0 = 0xFF;
      LED1 = 0xFF;
      LED2 = 0xFF;
      LED3 = (power - 0xBF);
    }
    else if(power > 0x7F) {
      LED0 = 0xFF;
      LED1 = 0xFF;
      LED2 = (power - 0x7F);
    }
    else if(power > 0x3F) {
      LED0 = 0xFF;
      LED1 = (power - 0x3F);
    }
    else {
      LED0 = (power & 0x3F);
    }
    
    if(dir == 0) {  
      colour0 = rearNeoStrip.Color(LED0, LED0, LED0);
      colour1 = rearNeoStrip.Color(LED1, LED1, LED1);
      colour2 = rearNeoStrip.Color(LED2, LED2, LED2);
      colour3 = rearNeoStrip.Color(LED3, LED3, LED3);
    }
    else {
      colour0 = rearNeoStrip.Color(LED0, 0, 0);
      colour1 = rearNeoStrip.Color(LED1, 0, 0);
      colour2 = rearNeoStrip.Color(LED2, 0, 0);
      colour3 = rearNeoStrip.Color(LED3, 0, 0);
    }
    
    if(i == 0) {
      rearNeoStrip.setPixelColor(4, colour0); 
      rearNeoStrip.setPixelColor(5, colour1); 
      rearNeoStrip.setPixelColor(6, colour2); 
      rearNeoStrip.setPixelColor(7, colour3);       
    }
    else {
      rearNeoStrip.setPixelColor(3, colour0); 
      rearNeoStrip.setPixelColor(2, colour1); 
      rearNeoStrip.setPixelColor(1, colour2); 
      rearNeoStrip.setPixelColor(0, colour3);       
    }
    
  }
  rearNeoStrip.show();    
}

