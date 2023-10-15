#include "MPU_Serial.h"

// Commands

#define COMMAND_SYMBOL '$'
#define COMMAND_STOP 1
#define COMMAND_READ_ANGLE 2
#define COMMAND_END_SERIAL 3

int current_command = COMMAND_STOP;

const int bufferSize = 32; // Define a buffer size for incoming data

char inputBuffer[bufferSize];  // Input buffer for reading data
char outputBuffer[bufferSize]; // Output buffer for storing the command

MPU_Serial mpuSerial;

int checkCommand(char *inputBuffer)
{
  if (inputBuffer[0] == COMMAND_SYMBOL)
  {
    char c = inputBuffer[1];
    return c - '0';
  }
  return current_command;
}

void doCommand(int command)
{
  switch (command)
  {
  case COMMAND_STOP:
    break;
  case COMMAND_READ_ANGLE:
    mpuSerial.printAngles();
    break;
  case COMMAND_END_SERIAL:
    mpuSerial.end();
    break;
  default:
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  mpuSerial.begin(9600);
  while (!Serial)
  {
  }
}

void loop()
{
  if (Serial.available() > 0)
  {
    // Read data from Serial and store it in the input buffer
    int bytesRead = Serial.readBytesUntil('\n', inputBuffer, bufferSize);
    inputBuffer[bytesRead] = '\0'; // Null-terminate the input buffer

    // strncpy(outputBuffer, inputBuffer, sizeof(outputBuffer));
    current_command = checkCommand(inputBuffer);

    // Clear the input buffer
    memset(inputBuffer, 0, sizeof(inputBuffer));
  }
  doCommand(current_command);
  delay(100);
}
