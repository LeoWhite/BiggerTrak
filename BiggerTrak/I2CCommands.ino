#include <Wire.h>


// List of the supported I2C commands
enum { 
  I2C_CMD_GET_STATE = 0xF,
  I2C_CMD_STOP = 0x11,
  I2C_CMD_SET_MOTORS = 0x12,
  I2C_CMD_AUTODRIVE = 0x13,
  I2C_CMD_AUTODRIVE_ROTATE = 0x14,
  I2C_CMD_SET_SERVO = 0x15
  
};

enum { 
  I2C_MSG_ARGS_MAX = 32,
  I2C_RESP_LEN_MAX = 32
};

typedef int (*i2cCallback)(byte *i2cArgs, uint8_t *pi2cResponse);

typedef struct i2cCommand {
  byte command;
  byte numberOfArgs;
  i2cCallback fnCallback;  
};


// Lookup table of all the supported commands.
// Detailing the command number, number of arguments
// and the function to call to proess it
extern const i2cCommand supportedI2Ccmd[] = { 
  { I2C_CMD_GET_STATE, 0, getStatus},
  { I2C_CMD_STOP, 0, motorsI2CStop},
  { I2C_CMD_SET_MOTORS, 4, motorsI2CSet},
  { I2C_CMD_AUTODRIVE, 4, autoDriveI2C},
  { I2C_CMD_AUTODRIVE_ROTATE, 2, autoDriveI2CRotate},
  { I2C_CMD_SET_SERVO, 2, servoI2CSet}
};

// The i2c address we will be using
#define SLAVE_ADDRESS 0x07

int argsCnt = 0;                        // how many arguments were passed with given command

byte i2cArgs[I2C_MSG_ARGS_MAX];         // array to store args received from master
int i2cArgsLen = 0;                     // how many args passed by master to given command

uint8_t i2cResponse[I2C_RESP_LEN_MAX];  // array to store response
int i2cResponseLen = 0;                 // response length


const i2cCommand *requestedCmd = NULL;

/**
 * Configures the i2c communication ready 
 * for use
 */
void I2CSetup() {
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);  
}

/**
 * Checks if there is a pending i2c command to process.
 */
void I2C_CheckCommands() {
  if(requestedCmd) {
    int extraArgs = 0;
    
    // The first argument is always the command number
    // so the callee can confirm its been executed
    i2cResponseLen = 0;
    i2cResponse[i2cResponseLen++] = requestedCmd->command;    
    
    // Trigger the callback function
    extraArgs = requestedCmd->fnCallback(i2cArgs, &i2cResponse[1]);
    i2cResponseLen += extraArgs;
    
    // Calculate the checksum for the response to
    // allow some simple error checking
    uint8_t CS = i2cResponseLen;
    for (int i = 0; i<i2cResponseLen; i++){
      CS^=i2cResponse[i];
    } 

    // Append the checksum to the end of the response arguments
    i2cResponse[i2cResponseLen++] = CS;  
    
    // Clear pointer so we don't trigger it twice
    requestedCmd = NULL;    
  }
}


// callback for received data
void receiveData(int howMany){
  int cmdRcvd = -1;
  int argIndex = -1; 
  int availableBytes = 0;
  argsCnt = 0;

  if(requestedCmd) {
    Serial.println("Command lost!");
  }

  if (howMany) {
    cmdRcvd = Wire.read();                 // receive first byte - command assumed
    howMany--; // Skip the 'command' byte
    while(howMany--){               // receive rest of tramsmission from master assuming arguments to the command
      if (argIndex < I2C_MSG_ARGS_MAX){
        argIndex++;
        i2cArgs[argIndex] = Wire.read();
      }
      else{
        ; // implement logging error: "too many arguments"
      }
      argsCnt = argIndex+1;  
    }
  }
  else{
    // implement logging error: "empty request"
    return;
  }
  
  // Calculate the checksum
  uint8_t CS = (argsCnt - 1) + 1;
  
  // Starts with the length, followed by the command, followed by all bits of the message
  // excluding the checksum
  CS^=cmdRcvd;
  for (int i = 0; i<(argsCnt - 1); i++){
    CS^=i2cArgs[i];
  } 
  
  // Does it match?
  if(CS != i2cArgs[argsCnt - 1]) {    
    // Perform a stop
    Serial.print("invalid checksum:");
    Serial.println(CS);
    MotorsStop();
    
    return;
  }
  
  // We don't include the checksum in the message
  argsCnt--;

  // validating command is supported by slave
  int fcnt = -1;
  for (int i = 0; i < sizeof(supportedI2Ccmd); i++) {
    if (supportedI2Ccmd[i].command == cmdRcvd && 
        supportedI2Ccmd[i].numberOfArgs == argsCnt) 
    {
      fcnt = i;
    }
  }

  if (fcnt<0){
    // implement logging error: "command not supported"
    Serial.println("not supported");
    return;
  }
  
  
  requestedCmd = &supportedI2Ccmd[fcnt];
  // now main loop code should pick up a command to execute and prepare required response when master waits before requesting response
}
// callback for sending data
void sendData(){  
  // Send the cached response
  Wire.write(i2cResponse, i2cResponseLen);
  
  // and clear the length so we don't send it again
  i2cResponseLen = 0;
}

