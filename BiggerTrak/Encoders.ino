
#define NO_PORTA_PINCHANGES // to indicate that port a will not be used for pin change interrupts
//#define NO_PORTB_PINCHANGES // to indicate that port b will not be used for pin change interrupts
#define NO_PORTC_PINCHANGES // to indicate that port c will not be used for pin change interrupts
#define NO_PORTD_PINCHANGES

#include <PinChangeInt.h>

/**
 * Setup and configude the encoders ready for use
 */
void encodersSetup() {
  // Configure motor encoder pins as inputs
  pinMode(lmencpin, INPUT)  ;
  digitalWrite(lmencpin, HIGH);
  pinMode(lmenc_secondpin, INPUT)  ;
  digitalWrite(lmenc_secondpin, HIGH);

  pinMode(rmencpin, INPUT);
  digitalWrite(rmencpin, HIGH);
  pinMode(rmenc_secondpin, INPUT);
  digitalWrite(rmenc_secondpin, HIGH);

  // Set up an interrupt callback for the encoder pins so
  // they update as soon as they change
  PCintPort::attachInterrupt(lmencpin, &leftEncoder, CHANGE);
  PCintPort::attachInterrupt(lmenc_secondpin, &leftEncoder, CHANGE);
  PCintPort::attachInterrupt(rmencpin, &rightEncoder, CHANGE);
  PCintPort::attachInterrupt(rmenc_secondpin, &rightEncoder, CHANGE);
}

/**
 * Callback when the left encoder interrupt fires.
 * This does the minimum work to ensure interrupts
 * are not disabled for too long
 */
void leftEncoder() {
  lmenc++;
}

/**
 * Callback when the right encoder interrupt fires.
 * This does the minimum work to ensure interrupts
 * are not disabled for too long
 */
void rightEncoder() {
  rmenc++;
}
