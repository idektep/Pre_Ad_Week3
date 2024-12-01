#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebSrv.h>
#include <sstream>  // for std::istringstream 

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 100
#define SERVOMAX 600

float playbackSpeed = 2.0; // ปรับความเร็วในการเล่นคำสั่ง (1.0 คือปกติ, มากกว่า 1.0 คือเร็วขึ้น, น้อยกว่า 1.0 คือช้าลง)

// Define the number of servos
#define NUM_SERVOS 4

// Create the PWM driver
Adafruit_PWMServoDriver idektep = Adafruit_PWMServoDriver();

int angleToPulse(int ang) {
  int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX);
  return pulse;
}

// Pin mappings and initial positions for each servo
struct ServoPins
{
  int servoPin;    // The PWM pin number for the servo
  String servoName;  // Name of the servo (Base, Shoulder, Elbow, Wrist_rotation)
  int initialPosition;  // Initial servo position (in degrees)
};

// Array to store servo information
std::vector<ServoPins> servoPins = 
{
  { 0, "Base", 90},
  { 1, "_____", 90},
  { ___, "____", ___},
  { ___, "Wrist_rotation", ___},
};

// Structure to store recorded steps for playback
struct RecordedStep {
  int servoIndex;
  int value;
  unsigned long delayInStep;
};

std::vector<RecordedStep> recordedSteps;  // Store the recorded steps

bool recordSteps = false;
bool playRecordedSteps = false;

unsigned long previousTimeInMilli = millis();

const char* ssid = "RobotArm";
const char* password = "__________";

AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");

void setup() {
  Wire.begin();  // Start I2C communication
  idektep.begin();   // Initialize the PWM Servo Driver
  idektep.setPWMFreq(60);  // Set the PWM frequency to 60Hz, ideal for servos
  
  Serial.begin(115200);
  
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  wsRobotArmInput.onEvent(onRobotArmInputWebSocketEvent);
  server.addHandler(&wsRobotArmInput);
  
  server.begin();
  Serial.println("HTTP server started");

  idektep.setPWM(0, 0, angleToPulse(___));
  idektep.setPWM(1, 0, angleToPulse(___));
  idektep.setPWM(2, 0, angleToPulse(___));
  idektep.setPWM(3, 0, angleToPulse(___));
  delay(1000);
}


void loop() {
  wsRobotArmInput.cleanupClients();
  
  if (playRecordedSteps) {
    playRecordedRobotArmSteps();
  }
}
