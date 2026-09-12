#include <Servo.h>

// 3 Ultrasonic Sensor Pins (Analog pins used as digital)
#define TRIG_FRONT A0
#define ECHO_FRONT A1

#define TRIG_LEFT  A2
#define ECHO_LEFT  A3

#define TRIG_RIGHT A4
#define ECHO_RIGHT A5

// L298N Motor Driver Pins
#define ENA 3   // Speed control Left Motor (PWM)
#define IN1 4   // Direction Left Motor
#define IN2 7   // Direction Left Motor
#define IN3 8   // Direction Right Motor
#define IN4 12  // Direction Right Motor
#define ENB 5   // Speed control Right Motor (PWM)

// Servo Pin
#define SERVO_PIN 11

Servo lidServo;

// Distance threshold
const int ESCAPE_THRESHOLD = 35; // Trigger distance in cm
const int MOTOR_SPEED = 220;     // Motor speed (0 - 255)

// Timed action settings (milliseconds)
const int ESCAPE_TIME = 5000;    // Duration to reverse when front obstacle is detected
const int TURN_TIME = 500;       // Duration to rotate away when side obstacle is detected

void setup() {
  Serial.begin(9600);

  // Ultrasonic Pins Setup
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  // Motor Driver Pins Setup
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Servo Setup
  lidServo.attach(SERVO_PIN);
  closeLid(); 
}

void loop() {
  long frontDist = readDistance(TRIG_FRONT, ECHO_FRONT);
  long leftDist  = readDistance(TRIG_LEFT, ECHO_LEFT);
  long rightDist = readDistance(TRIG_RIGHT, ECHO_RIGHT);

  Serial.print("Front: "); Serial.print(frontDist);
  Serial.print(" cm | Left: "); Serial.print(leftDist);
  Serial.print(" cm | Right: "); Serial.print(rightDist);
  Serial.println(" cm");

  // 3-Sensor Evasion Logic with dedicated movement duration
  if (frontDist < ESCAPE_THRESHOLD) {
    closeLid();
    moveBackward();
    delay(ESCAPE_TIME); // Run backward for set time
    stopMotors();       // Stop after moving far enough
  } 
  else if (leftDist < ESCAPE_THRESHOLD) {
    closeLid();
    turnRight();        // Turn clockwise away from left obstacle
    delay(TURN_TIME);   // Turn for set time
    stopMotors();       // Stop after turning far enough
  } 
  else if (rightDist < ESCAPE_THRESHOLD) {
    closeLid();
    turnLeft();         // Turn counter-clockwise away from right obstacle
    delay(TURN_TIME);   // Turn for set time
    stopMotors();       // Stop after turning far enough
  } 
  else {
    stopMotors();
  }

  delay(30);
}

// Ultrasonic Measurement Function
long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return 400;

  return duration * 0.0343 / 2;
}

// Servo Lid Control
void closeLid() {
  lidServo.write(0); 
}

// Navigation Routines
void moveBackward() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
