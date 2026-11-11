/*
 * Smart Automated Cutter - Arduino Uno firmware (Team 15)

 * Libraries: AFMotor (Adafruit Motor Shield v1 / L293D), HUSKYLENS (DFRobot), Wire, Servo
 */

#include <AFMotor.h>
#include <HUSKYLENS.h>
#include <Wire.h>
#include <Servo.h>

Servo myservo;
int servopin = 9;

// 1,2 right
// 3,4 left
// general order - 1,3 left and 2,4 right
// replace 4 with 1 and 1 with 4

// Motor initialization
AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR34_64KHZ);
AF_DCMotor motor4(4, MOTOR34_64KHZ);

// HuskyLens initialization
HUSKYLENS huskylens;

// PID parameters (Ki = Kd = 0, so effectively proportional-only)
float Kp = 4, Ki = 0, Kd = 0;
int lastError = 0;
int integral = 0;

void setup() {
  Serial.begin(115200);

  // Motor speed initialization (0-255)
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);

  pinMode(13, OUTPUT);

  // function call for Servo height
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  analogWrite(A0, LOW);
  analogWrite(A1, LOW);
  analogWrite(A2, LOW);
  Serial.begin(9600);
  myservo.attach(servopin);
  servoheight();

  // HuskyLens communication setup
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS not detected!"));
    delay(1000);
  }
  huskylens.writeAlgorithm(ALGORITHM_LINE_TRACKING);
  Serial.println(F("Line tracking mode activated"));
}

void loop() {
  if (huskylens.request()) {
    HUSKYLENSResult result = huskylens.read();

    if (result.command == COMMAND_RETURN_ARROW) {
      int lineCenterX = result.xTarget;
      int error = lineCenterX - 160;  // 160 = center of 320px wide image

      // PID calculation
      int correction = calculatePID(error);

      // Motor control logic with differential steering
      int leftSpeed = 200 + correction;
      int rightSpeed = 200 - correction;

      // Constrain speeds and handle direction changes
      leftSpeed = constrain(leftSpeed, -255, 255);
      rightSpeed = constrain(rightSpeed, -255, 255);

      setMotorSpeeds(leftSpeed, rightSpeed);

    } else {
      stopMotors();
    }
  } else {
    stopMotors();
  }
}

int calculatePID(int error) {
  integral += error;
  int derivative = error - lastError;
  lastError = error;

  return (Kp * error) + (Ki * integral) + (Kd * derivative);
}

void setMotorSpeeds(int left, int right) {
  // Left motors control
  if (left >= 0) {
    motor4.run(FORWARD);
    motor3.run(FORWARD);
    motor4.setSpeed(left);
    motor3.setSpeed(left);
  } else {
    motor4.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.setSpeed(-left);
    motor3.setSpeed(-left);
  }

  // Right motors control
  if (right >= 0) {
    motor2.run(FORWARD);
    motor1.run(FORWARD);
    motor2.setSpeed(right);
    motor1.setSpeed(right);
  } else {
    motor2.run(BACKWARD);
    motor1.run(BACKWARD);
    motor2.setSpeed(-right);
    motor1.setSpeed(-right);
  }
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

// Blocks until the "stop"/confirm button (A2) is pressed.
void servoheight()
{
  int cw, acw, st;
  int flag = 1;
  while (flag)
  {
    cw  = digitalRead(A0);
    acw = digitalRead(A1);
    st  = digitalRead(A2);
    if ((cw == 1) && (acw == 0) && (st == 0))
    {
      myservo.write(0);
      digitalWrite(13, LOW);
      // Kp = Kp + 0.1;
    }
    else if ((cw == 0) && (acw == 1) && (st == 0))
    {
      myservo.write(180);
      digitalWrite(13, HIGH);
      // Kd = Kd + 0.01;
    }
    else if ((cw == 0) && (acw == 0) && (st == 1))
    {
      // myservo.write(0);
      Serial.println("Stopped");
      flag = 0;
      delay(3000);
    }
    else
    {
      myservo.write(90);
    }
  }
  return;
}
