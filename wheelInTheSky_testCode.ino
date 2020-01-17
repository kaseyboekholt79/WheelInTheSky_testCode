// TEST CODE
// This code allows state (on/off), speed, direction, acceleration, and deceleration control
// of the wheel in the sky's stepper motor via the serial monitor. It also allows for motor
// diagnostic readouts

#include <AccelStepper.h>

String inputString = "";         // a string to hold incoming serial data
byte motState = 0;               // initialize the motor to off
int const maxWheelSpeed = 1000;  // set max motor speed value
int const maxWheelAccel = 500;   // set max motor acceleration/decelleration rate
float destination = 10 ^ 8;      // target position, for testing we want to run the wheel indefinitely
bool wheelDirection = false;          // used to change the direction of the wheel

AccelStepper wheelStepperMotor(1, 8, 9);  // create stepper motor object
// 1 for DRIVER mode
// arduino pin 8 for the STEP signal
// arduini pin 9 for the DIR signal

void setup()  {
  Serial.begin(57600);
  wheelStepperMotor.setMaxSpeed(maxWheelSpeed);       // calls setMaxSpeed public member function
  wheelStepperMotor.setAcceleration(maxWheelAccel);   // calls setAcceleration public member function
}

void loop() {
  wheelStateMachine();           // calls the stepper motor state machine function
  wheelStepperMotor.run();       // moves the motor, implementing accelerations/decelerations to acheive target position
}

void wheelStateMachine() {       // stepper motor state machine, either running or not running
  switch (motState) {
    case 0:
      wheelStepperMotor.stop();  // causes the stepper to stop as quickly as possible, using the current speed and acceleration parameters
      break;

    case 1:
      wheelStepperMotor.moveTo(destination);   // The run() function will move the motor (one step per call) from
      // the current position to the target position set by  the moveTo function
      break;
  }
}

void serialEvent() {                     //this function triggers when there is an inbound serial message
  while (Serial.available()) {
    char inChar = (char)Serial.read();   // get the new byte
    if (inChar == '\n') {                // if the incoming character is a newline call messageAction function
      String incoming = inputString;
      inputString = "";
      messageAction(incoming);
    }
    else if (inChar == '\r') {}
    else if (!isPrintable(inChar)) {}
    else {
      inputString += inChar;             // add any incoming characters to the inputString
    }
  }
}

void messageAction(String incoming) {
  if (incoming.startsWith("PING")) {                 // test for serial connectivity
    Serial.println("PONG");
  }

  if (incoming.startsWith("STATE")) {                // turn motor on/off
    byte inputState = (String(incoming.charAt(5))).toInt();
    if (inputState == 0 || inputState == 1) {
      motState = inputState;
    }
    Serial.print("motor state: ");
    Serial.println(motState);
  }

  if (incoming.startsWith("SPEED")) {                // set/change motor speed
    int maxWheelSpeed = (String(incoming.charAt(5)) + String(incoming.charAt(6))
                         + String(incoming.charAt(7)) + String(incoming.charAt(8))).toInt();
    wheelStepperMotor.setMaxSpeed(maxWheelSpeed);
    Serial.print("motor speed: ");
    Serial.println(maxWheelSpeed);
  }

  if (incoming.startsWith("ACCEL")) {               // set/change motor acceleration/deceleration
    int maxWheelAccel = (String(incoming.charAt(5)) + String(incoming.charAt(6))
                         + String(incoming.charAt(7))).toInt();
    wheelStepperMotor.setAcceleration(maxWheelAccel);
    Serial.print("motor acceleration: ");
    Serial.println(maxWheelAccel);
  }

  if (incoming.startsWith("DIR")) {                  // set/change motor direction
    byte newDirection = (String(incoming.charAt(3))).toInt();
    if (newDirection != wheelDirection) {
      wheelDirection = newDirection;
      destination = 10^8;
      if (wheelDirection) {
        destination = -10^8;
      }
      wheelStepperMotor.stop();                      // allow time for wheel to come to rest before changing direction
      delay(500);
      if (motState) {
        delay(2000);
      }
    }
    Serial.print("motor direction: ");
    Serial.println(wheelDirection);
  }

  if (incoming.startsWith("REPORT")) {               // print out a diagnostic report
    Serial.print("State: ");
    Serial.println(motState);
    Serial.print("Direction: ");
    Serial.println(wheelDirection);
    Serial.print("Speed: ");
    Serial.println(maxWheelSpeed);
    Serial.print("Acceleration: ");
    Serial.println(maxWheelAccel);
    Serial.print("Position: ");
    Serial.println(wheelStepperMotor.currentPosition());
    Serial.print("Distance to Go: ");
    Serial.println(wheelStepperMotor.distanceToGo());
    Serial.println("");
  }

}
