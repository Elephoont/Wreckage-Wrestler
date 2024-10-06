#include <SoftwareSerial.h>
#include <ODriveUART.h>

#define SAFETY_BUTTON_PIN 2
#define WIN_BUTTON_PIN 3
#define LOSE_BUTTON_PIN 4

SoftwareSerial ODriveSerial(10, 11); 
int level;
static int velocity = 100;

ODriveUART odrive(ODriveSerial);

void setup() {
  // put your setup code here, to run once:
  // setting up button pins
  pinMode(SAFETY_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LOSE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WIN_BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  // Start communicating with the ODrive Mini 
  long baudRates[] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200};

  for (int i = 0; i < sizeof(baudRates) / sizeof(baudRates[0]); i++) {
    long baudRate = baudRates[i];
    ODriveSerial.begin(baudRate); // Set new baud rate
    delay(1000); // Wait for the ODrive to adjust

    // Send a simple command (e.g., asking for state)
    Serial.print("attempting ");
    Serial.println(baudRate);
    ODriveSerial.print("r axis0 encoder \n"); // Example command to request encoder state
    delay(100); // Allow time for ODrive to respond

    // Read response
    if (ODriveSerial.available()) {
      String response = ODriveSerial.readStringUntil('\n');
      Serial.print("Baud Rate: ");
      Serial.print(baudRate);
      Serial.print(" - Response: ");
      Serial.println(response); // Print the response from ODrive
    } else {
      Serial.print("Baud Rate: ");
      Serial.print(baudRate);
      Serial.println(" - No response");
    }
  }

  
  // ODriveSerial.begin(9600);
  // Serial.println("Communication with ODrive started.");

  // while(odrive.getState() == AXIS_STATE_UNDEFINED){
  //   delay(1000);
  // }

  // Serial.println("Found ODrive");

  level = 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(SAFETY_BUTTON_PIN) == LOW && digitalRead(WIN_BUTTON_PIN) == HIGH && digitalRead(LOSE_BUTTON_PIN) == HIGH){ // When the safety button is pressed and the win & lose buttons are NOT pressed, begin arm wrestling
    Serial.println("Safety button pressed. Starting the motor in 3 seconds");
    delay(3000);
    ODriveSerial.print("v 0 " + String(velocity * level) + "\n");
    delay(1000);
  } else {
    ODriveSerial.print("z 0 0\n"); // shut motor off
    Serial.println("At endstop. Shutting motor off");
    delay(1000);
  }

  if(digitalRead(WIN_BUTTON_PIN) == LOW){
    Serial.println("WIN, moving to next level.");
    delay(10000); // wait 10 seconds before advancing
    level++;
  }

  if(digitalRead(LOSE_BUTTON_PIN) == LOW){
    Serial.println("LOSS, resetting.");
    delay(10000); // wait 10 seconds before advancing 
    level = 1;
  }
}
