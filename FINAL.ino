#define BLYNK_TEMPLATE_ID "TMPL6bD2KEXTs"
#define BLYNK_TEMPLATE_NAME "Motor"
#define BLYNK_AUTH_TOKEN "llygsdgSIVl1QGE-T_EAcK2pLmizNi8u"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;  // Replace "YourAuthToken" with your Blynk authentication token
char ssid[] = "SLT_FIBER";   // Replace "YourWiFiSSID" with your WiFi SSID
char pass[] = "Chootiya1234";  // Replace "YourWiFiPassword" with your WiFi password

#define RAIN_SENSOR_PIN 12
#define EN_PIN   19 // Enable 
#define STEP_PIN  18 // Step
#define DIR_PIN   4  // Direction
#define SIM800_TX_PIN 17
#define SIM800_RX_PIN 16

BlynkTimer timer;
RTC_DS3231 rtc;


void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // Deactivate driver (LOW active)
  
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW); // Set the direction pin to low
  
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW); // Set the step pin to low
  digitalWrite(EN_PIN, LOW);
  pinMode (V1, OUTPUT);
  pinMode (V2, OUTPUT);
  Blynk.begin(auth, ssid, pass);
}

int flag = 0;
void loop() {
  int isRaining = digitalRead(RAIN_SENSOR_PIN);

  if (isRaining == LOW && flag == 0) {
    rotateMotor(875, 0, 1100);
    Serial.println("Its Raining");
    sendSMS("Window closed due to rain!"); // Rotate clockwise to close the window
    flag = 1;
    delay(1000);
  } else if (isRaining == HIGH && flag == 1) {
    flag = 0;
  }

  DateTime now = rtc.now();
  if (now.hour() ==1 && now.minute() == 27 && now.second() == 0) {
    // Rotate the motor clockwise
    rotateMotor(875, 0, 1100);
    sendSMS("Window closed as scheduled!");
    delay(100); // Adjust as needed
  }
  delay(100);
  Blynk.run();
  timer.run();
}

// Function to rotate the motor
void rotateMotor(int steps, bool direction, int speed) {
  // Set the motor direction
  digitalWrite(DIR_PIN, direction);
  
  // Step the motor
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(speed);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(speed);
  }
}

SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
void sendSMS(String message) {
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"+94771143079\"\r\n");
  delay(1000);
   
  //Send SMS content
  serialSIM800.print(message);
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
}

// Blynk app button to move the motor clockwise
// Blynk app button to move the motor clockwise
BLYNK_WRITE(V1) {
  int buttonState = param.asInt();
  if (buttonState == 1) { // If button is pressed
    rotateMotor(925, 1, 1100); // Rotate clockwise
    sendSMS("Window opened!");
    digitalWrite(EN_PIN, LOW);
    
     // Activate driver
  }
}

// Blynk app button to move the motor counterclockwise
BLYNK_WRITE(V2) {
  int buttonState = param.asInt();
  if (buttonState == 1) { // If button is pressed
    rotateMotor(875, 0, 1100); // Rotate counterclockwise
    sendSMS("Window closed!");
    digitalWrite(EN_PIN, LOW);
    
    
     // Activate driver
  }


}