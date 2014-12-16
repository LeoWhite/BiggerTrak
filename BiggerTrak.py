#!/usr/bin/env python
 
import pygame
import wiringpi2 as wiringpi
import time
import os, sys, math
from collections import namedtuple
import struct

# Key mappings
PS3_SELECT = 0
PS3_L3 = 1
PS3_R3 = 2
PS3_START = 3
PS3_DPAD_UP = 4
PS3_DPAD_RIGHT = 5
PS3_DPAD_DOWN = 6
PS3_DPAD_LEFT = 7
PS3_L2 = 8
PS3_R2 = 9
PS3_L1 = 10
PS3_R1 = 11
PS3_TRIANGLE = 12
PS3_CIRCLE = 13
PS3_CROSS =  14
PS3_SQUARE = 15
PS3_PLAYSTATION = 16

PS3_AXIS_LEFT_H = 0
PS3_AXIS_LEFT_V = 1
PS3_AXIS_RIGHT_H = 2
PS3_AXIS_RIGHT_V = 3
PS3_AXIS_DPAD_UP = 8
PS3_AXIS_DPAD_RIGHT = 9
PS3_AXIS_DPAD_DOWN = 10
PS3_AXIS_DPAD_LEFT = 11 
PS3_AXIS_L2 = 12
PS3_AXIS_R2 = 3
PS3_AXIS_L1 = 14
PS3_AXIS_R1 = 15
PS3_AXIS_TRIANGLE = 16
PS3_AXIS_CIRCLE = 17
PS3_AXIS_CROSS =  18
PS3_AXIS_SQUARE = 19
PS3_AXIS_PLAYSTATION = 16

# The I2C address of the arduino
I2CAddress = 0x07

# Initialise the pygame library, ready for use
pygame.init()

# Define the status structure
StatusStruct = namedtuple("StatusStruct", "start errorFlag batteryVoltage leftMotorCurrent leftMotorEncoder rightMotorCurrent rightMotorEncoder xAxis yAxis zAxis deltaX deltaY deltaZ")

# sends the i2c command to the Arduino, automatically adding a checksum
#
# cmd - the byte command to send
# message - any extra arguments to send
def sendMessage(cmd, message):
  newMessage =  cmd + message
  
  # Calculate checksum
  CS = len(newMessage)
  for i in range(0, len(newMessage)):
    CS = (CS & 0xFF ^ ord(newMessage[i:i+1]) & 0xFF)
    
  finalMessage = newMessage + chr(CS)
  
  # Send the message  
  os.write(i2cConnect, finalMessage)
  
# reads a message back from the Arduino, vaidating
# the checksum
#
# length - The expected length of the message  
def readMessage(length):
    # Read in data plus checksum
    status = i2cFD.read(length + 1)
    
    if len(status) == (length + 1):
      CS = length;
      for i in range(0, length):
        CS = (CS & 0xFF ^ ord(status[i:i+1]) & 0xFF)
      
      if CS == ord(status[length:length+1]):
        # Process checksum,
        return status[:length]
      else:
        print "Checksum mismatch!"
        
    return 0

        
# output the current status of the robot
def outputStatus():
  global i2cConnect, i2cFD
  
  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    sendMessage("\x0F", "")

    # Wait for the response to be generated, then read it in
    time.sleep(0.001)
    status = readMessage(24)
  
    # Unpack the message and print it
    currentStatus = StatusStruct._make(struct.unpack('!bbHhhhhhhhhhh', status))
    print currentStatus
  except:
    print "Failed to read status"
    i2cFD.flush()
  
# Stops the robot
def stop():
  global i2cConnect, i2cFD

  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    # Send the 'stop' request
    sendMessage("\x11", "")
    time.sleep(0.001)
    result = readMessage(1);
    if ord(result[0:1]) != 0x11:
      print "Failed to stop!"
  except:
    print "Failed to stop motors!"
    i2cFD.flush()

    
# Set the motor power to the specifiedvalues
#
# leftMotor - Power for left motor. -100 to 100
# rightMotor - Power for right motor. -100 to 100
def setMotors(leftMotor, rightMotor):
  global i2cConnect, i2cFD
  
  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    # We are sending two arguments, so we use the struct.pack code
    # to convert them into a byte stream
    sendMessage("\x12", struct.pack("!hh", leftMotor, rightMotor))
    time.sleep(0.001)
    result = readMessage(1);
    if ord(result[0:1]) != 0x12:
      print "Failed to set motors!"
  except:
    print "Failed to set motors!"
    i2cFD.flush()
    

# Set the position of the front mounted servo
#
# servoPosition - The ms value to set the servo position to.
def setServo(servoPosition):
  global i2cConnect, i2cFD
  
  # We use a try/catch block in case there is an error reading or writing
  # to the i2c file handle
  try:
    # Need to send one argument with the message
    sendMessage("\x15", struct.pack("!h", servoPosition))
    time.sleep(0.001)
    result = readMessage(1);
    if ord(result[0:1]) != 0x15:
      print "Failed to set servo!"
  except:
    print "Failed to set servo!"
    i2cFD.flush()

# Wait for a joystick to become available before running the
# rest of the script
while pygame.joystick.get_count() == 0:
  print 'waiting for joystick count = %i' % pygame.joystick.get_count()
  pygame.joystick.quit()
  time.sleep(1)
  pygame.joystick.init()


# Get a handle on the joystick
j = pygame.joystick.Joystick(0)
j.init()

print 'Initialized Joystick : %s' % j.get_name()

LeftTrack = 0
RightTrack = 0
PowerLimiter = 60
ServoPosition = 2500

# Configure wiring pi  
wiringpi.wiringPiSetupPhys()

# Open the i2C connection 
i2cConnect = wiringpi.wiringPiI2CSetup(I2CAddress)
i2cFD = os.fdopen(i2cConnect, "rw", 0)

try:
    # Only allow axis and button events
    pygame.event.set_allowed([pygame.JOYAXISMOTION, pygame.JOYBUTTONDOWN])

    # Make sure the motors are stopped
    stop()

    # Set the default servo position
    setServo(ServoPosition);
    
    # Read in current status
    outputStatus()
    
    while True:
        events = pygame.event.get()
        for event in events:
          UpdateMotors = 0
          UpdateServo = 0
          NewLeftTrack = 0;
          NewRightTrack = 0;
          
          # Check if the analogue sticks have changed position
          # and update the motors accordingly
          if event.type == pygame.JOYAXISMOTION:
            if event.axis == PS3_AXIS_LEFT_V:
              NewLeftTrack = -(math.ceil(event.value * PowerLimiter))
              if NewLeftTrack != LeftTrack:
                LeftTrack = NewLeftTrack
                UpdateMotors = 1
            elif event.axis == PS3_AXIS_RIGHT_V:
              NewRightTrack = -(math.ceil(event.value * PowerLimiter))
              if NewRightTrack != RightTrack:
                RightTrack = NewRightTrack
                UpdateMotors = 1

          if event.type == pygame.JOYBUTTONDOWN:
            # Moving the servo down
            if event.button == PS3_R1:
              if ServoPosition < 2500:
                ServoPosition += 100;
                UpdateServo = 1
            # Moving the servo up
            elif event.button == PS3_R2:
              if ServoPosition > 800:
                ServoPosition -= 100;
                UpdateServo = 1
            # Change the power limit to its default
            elif event.button == PS3_DPAD_LEFT:
              PowerLimiter = 60;
            # Change the power limit to its max
            elif event.button == PS3_DPAD_RIGHT:
              PowerLimiter = 100;
            # Increase the power limit
            elif event.button == PS3_DPAD_UP:
              PowerLimiter += 10;
              if PowerLimiter > 100:
                PowerLimiter = 100;
            # Increase the power limit
            elif event.button == PS3_DPAD_DOWN:
              PowerLimiter -= 10;
              if PowerLimiter < 60:
                PowerLimiter = 60;
                
          if UpdateMotors:
            #print 'LeftTrack %f' % LeftTrack
            #print 'RightTrack %f' % RightTrack              
            setMotors(LeftTrack, RightTrack)  
              
          if UpdateServo:
            setServo(ServoPosition)
            
        
        
except KeyboardInterrupt:
    # Turn off the motors
    stop()
    j.quit()
