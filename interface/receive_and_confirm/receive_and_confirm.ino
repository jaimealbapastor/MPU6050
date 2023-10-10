const int bufferSize = 128; // Define a buffer size for incoming data

char inputBuffer[bufferSize]; // Input buffer for reading data
char outputBuffer[bufferSize]; // Output buffer for storing the command

void setup() {
  Serial.begin(9600); // Initialize Serial communication
}

void loop() {
  if (Serial.available() > 0) {
    // Read data from Serial and store it in the input buffer
    int bytesRead = Serial.readBytesUntil('\n', inputBuffer, bufferSize);
    inputBuffer[bytesRead] = '\0'; // Null-terminate the input buffer

    strncpy(outputBuffer, inputBuffer, sizeof(outputBuffer));
    if (inputBuffer[0]=='0'){
      Serial.end();
    }
    
    // Print the received command
    Serial.print("Received: ");
    Serial.println(inputBuffer);

    // Print the command from the output buffer
    Serial.print("Sent: ");
    Serial.println(outputBuffer);

    // Clear the input buffer
    memset(inputBuffer, 0, sizeof(inputBuffer));
  }
}
