#include "Serial_Commands.h"
#include "motor_drivers.h"
#include "encoders.h"
#include "US_Sensor_Detection.h"

#include <Arduino.h>
#include <UcTTDcMotor.h>
#include <UcMotorPWM.h>

// Motor objects defined in motor_drivers.cpp
extern UcTTDcMotor motorA;
extern UcTTDcMotor motorB;


// ============================================================
// SETTINGS
// ============================================================

#define DUTY_RATIO 75.0

// Your experimentally determined relationship:
// approximately 2.6 RPM per 1% duty ratio

#define RPM_PER_PERCENT 2.6

// Wheel diameter
#define WHEEL_DIAMETER_CM 6.6
#define WHEEL_TRACK_CM 13.0


// ============================================================
// VARIABLES
// ============================================================

unsigned long brakeTime;

bool serialControl = false;
bool commandFlag = false;

static unsigned long prevTime_distance = 0;
static bool moving_distance = false;

static unsigned long prevTime_angle = 0;
static bool rotating_angle = false;

unsigned long period_cm = 0;
unsigned long period_angle = 0;

static unsigned long buzzerPrevDuration_Distance = 0;
static unsigned long buzzerPrevDuration_Angle = 0;


// ============================================================
// CALCULATE MOTOR RPM FROM DUTY RATIO
// ============================================================

float calculate_rpm()
{
    float rpm;

    rpm = RPM_PER_PERCENT * DUTY_RATIO;

    return rpm;
}


// ============================================================
// CALCULATE TIME REQUIRED FOR DISTANCE
// ============================================================
//
// Input:
//     cm = desired distance in centimetres
//
// Output:
//     time in milliseconds
//
// Formula:
//
//     wheel circumference = PI * D
//
//     speed (cm/min) = circumference * RPM
//
//     speed (cm/s) = circumference * RPM / 60
//
//     time(s) = distance / speed
//
//     time(ms) = time(s) * 1000
//
// ============================================================

unsigned long period_calculation_cm(int cm)
{
    float rpm;
    float wheel_circumference_cm;
    float speed_cm_per_sec;

    float time_sec;
    float time_ms;

    rpm = calculate_rpm();

    wheel_circumference_cm = PI * WHEEL_DIAMETER_CM;

    speed_cm_per_sec = (wheel_circumference_cm * rpm) / 60.0;

    // Prevent division by zero
    if (speed_cm_per_sec <= 0.0)
    {
        return 0;
    }

    time_sec = (float)cm / speed_cm_per_sec;

    time_ms = time_sec * 1600.0;

    return (unsigned long)time_ms;
}


// ============================================================
// CALCULATE TIME REQUIRED FOR ROTATION
// ============================================================
//
// Based on your existing empirical equation:
//
//     time(s) = degrees / (6 * RPM)
//
// Convert seconds to milliseconds.
//
// ============================================================

unsigned long period_calculation_angle(int deg)
{
    float rpm;
    float wheel_circumference_cm;
    float speed_cm_per_sec;
    float angle_rad;
    float time_sec;
    float time_ms;

    rpm = calculate_rpm();

    if (rpm <= 0.0)
    {
        return 0;
    }

    wheel_circumference_cm =
        PI * WHEEL_DIAMETER_CM;

    speed_cm_per_sec =
        (wheel_circumference_cm * rpm) / 60.0;

    angle_rad =
        (float)deg * PI / 180.0;

    time_sec =
        (angle_rad * WHEEL_TRACK_CM)
        / (2.0 * speed_cm_per_sec);

    time_ms = time_sec * 1666.0; // 4000.0

    return (unsigned long)time_ms;
}

void serial_commands()
{
    // --------------------------------------------------------
    // Check for a new serial command
    // --------------------------------------------------------

    if (Serial.available())
    {
        String cmd = Serial.readStringUntil('\n');

        cmd.trim();

        int spaceIndex = cmd.indexOf(' ');

        String action;
        int value = 0;


        // ----------------------------------------------------
        // Command contains a value
        //
        // Example:
        // F 10
        // L 90
        // ----------------------------------------------------

        if (spaceIndex != -1)
        {
            action = cmd.substring(0, spaceIndex);

            value =
                cmd.substring(spaceIndex + 1).toInt();
        }

        // ----------------------------------------------------
        // Command contains no value
        //
        // Example:
        // S
        // ----------------------------------------------------

        else
        {
            action = cmd;
        }


        // Convert command to uppercase
        action.toUpperCase();


        // ----------------------------------------------------
        // Execute command
        // ----------------------------------------------------

        turtle_motion(action, value);
    }


    // ========================================================
    // ULTRASONIC SENSOR
    // ========================================================

    //sensor_measure_distance();


    // --------------------------------------------------------
    // Object detected
    // --------------------------------------------------------

    if (objectDetected)
    {
        motorA.stop();
        motorB.stop();

        stateA = BRAKE_A;
        stateB = BRAKE_B;

        brakeTime = millis();

        moving_distance = false;
        rotating_angle = false;
    }


    // ========================================================
    // DISTANCE TIMER
    // ========================================================

    if (moving_distance &&
        (millis() - prevTime_distance >= period_cm))
    {
        moving_distance = false;

        motorB.stop();
        motorA.stop();

        buzzer_on();

        sensor_on();
        
    }


    // ========================================================
    // ANGLE TIMER
    // ========================================================

    if (rotating_angle &&
        (millis() - prevTime_angle >= period_angle))
    {
        rotating_angle = false;

        motorA.stop();
        motorB.stop();

        if(millis() - buzzerPrevDuration_Angle >= 300){
            buzzerPrevDuration_Angle = millis();
            buzzer_on();
        }

        sensor_on();
   
    }
}


// ============================================================
// TURTLE MOTION
// ============================================================

void turtle_motion(String action, int value)
{
    // ========================================================
    // FORWARD
    // ========================================================

    if (action == "F" || action == "FORWARD")
    {
        // Calculate required time
        period_cm = period_calculation_cm(value);

        Serial.print("FORWARD ");
        Serial.print(value);
        Serial.print(" cm");

        Serial.print(" | Duty = ");
        Serial.print(DUTY_RATIO);

        Serial.print("% | Estimated RPM = ");
        Serial.print(calculate_rpm());

        Serial.print(" | Period = ");
        Serial.print(period_cm);

        Serial.println(" ms");


        // ----------------------------------------------------
        // Start motors
        // ----------------------------------------------------

        motorB.reverse(DUTY_RATIO);
        motorA.reverse(DUTY_RATIO);

        prevTime_distance = millis();

        moving_distance = true;

        serialControl = true;

        sensor_off();
   
    }


    // ========================================================
    // BACKWARD
    // ========================================================

    else if (action == "B" || action == "BACKWARD")
    {
        // Calculate required time
        period_cm = period_calculation_cm(value);

        Serial.print("BACKWARD ");
        Serial.print(value);
        Serial.print(" cm");

        Serial.print(" | Duty = ");
        Serial.print(DUTY_RATIO);

        Serial.print("% | Estimated RPM = ");
        Serial.print(calculate_rpm());

        Serial.print(" | Period = ");
        Serial.print(period_cm);

        Serial.println(" ms");


        // ----------------------------------------------------
        // Start motors
        // ----------------------------------------------------

        motorB.forward(DUTY_RATIO);
        motorA.forward(DUTY_RATIO);

        prevTime_distance = millis();

        moving_distance = true;

        serialControl = true;

        sensor_off();

    }


    // ========================================================
    // LEFT
    // ========================================================

    else if (action == "L" || action == "LEFT")
    {
        // Calculate required rotation time
        period_angle = period_calculation_angle(value);

        Serial.print("LEFT ");
        Serial.print(value);
        Serial.print(" degrees");

        Serial.print(" | Duty = ");
        Serial.print(DUTY_RATIO);

        Serial.print("% | Estimated RPM = ");
        Serial.print(calculate_rpm());

        Serial.print(" | Period = ");
        Serial.print(period_angle);

        Serial.println(" ms");


        // ----------------------------------------------------
        // Start motors
        // ----------------------------------------------------

        motorA.forward(DUTY_RATIO);
        motorB.reverse(DUTY_RATIO);

        prevTime_angle = millis();

        rotating_angle = true;

        serialControl = true;

        sensor_off();

    }


    // ========================================================
    // RIGHT
    // ========================================================

    else if (action == "R" || action == "RIGHT")
    {
        // Calculate required rotation time
        period_angle = period_calculation_angle(value);

        Serial.print("RIGHT ");
        Serial.print(value);
        Serial.print(" degrees");

        Serial.print(" | Duty = ");
        Serial.print(DUTY_RATIO);

        Serial.print("% | Estimated RPM = ");
        Serial.print(calculate_rpm());

        Serial.print(" | Period = ");
        Serial.print(period_angle);

        Serial.println(" ms");


        // ----------------------------------------------------
        // Start motors
        // ----------------------------------------------------

        motorA.reverse(DUTY_RATIO);
        motorB.forward(DUTY_RATIO);

        prevTime_angle = millis();

        rotating_angle = true;

        serialControl = true;

        sensor_off();

    }


    // ========================================================
    // STOP
    // ========================================================

    else if (action == "S" || action == "STOP")
    {
        serialControl = false;

        moving_distance = false;

        rotating_angle = false;

        sensor_on();
        buzzer_off();

        motorA.stop();
        motorB.stop();

        Serial.println("STOP");
    }


    // ========================================================
    // UNKNOWN COMMAND
    // ========================================================

    else
    {
        Serial.print("Unknown command: ");
        Serial.println(action);
    }
}

/*#include "Serial_Commands.h"
#include "motor_drivers.h"
#include "encoders.h"
#include "US_Sensor_Detection.h"


#include <Arduino.h>
#include <UcTTDcMotor.h>
#include <UcMotorPWM.h>

// Motor objects defined in motor_drivers.cpp
extern UcTTDcMotor motorA;
extern UcTTDcMotor motorB;

bool serialControl;
bool commandFlag = false;

#define DUTY_RATIO 50 // Adjust the duty ratio to calculate the period required to reach a target distance in (cm)

static unsigned long prevTime_distance = 0;
static bool moving_distance = false;

static unsigned long prevTime_angle = 0;
static bool rotating_angle = false;

unsigned long period_cm = 0;
unsigned long period_angle = 0;

void serial_commands(){

  if(Serial.available() > 0){ //Determine whether a message is waiting

    String command = Serial.readStringUntil('\n'); // Take that waiting message and utilize it
    command.trim(); // Remove leading and trailing whitespaces after the string command

    // Ex 90 degrees
    // Ex 50 cm

    int firstSpace = command.indexOf(" "); // Space

    String firstString = command.substring(0, firstSpace); // 90 50
    firstString.trim(); // Remove leading and trailing whitespaces after the string command

    int value = firstString.toInt(); // Converts the number (string format) to the number (decimal)

    String unit = command.substring(firstSpace + 1); // degrees cm
    unit.trim(); // Remove leading and trailing whitespaces after the string command

    turtle_motion(command);

    if(unit == "cm" || unit == "CM"){

      // Calculates the period to move the robot at the given input distance --> Assume that the duty ratio is fixed at 50

      float period_calculation_cm = (100.0 * value)/((11.0 * PI) * AVG_SPEED_RPM);
      period_cm = (unsigned long)period_calculation_cm; // Convert the period in floating point back to int

      if (!moving_distance)
      {
          // Start motors
          motorA.forward(DUTY_RATIO);
          motorB.forward(DUTY_RATIO);

          prevTime_distance = millis(); // Start distance timer
          moving_distance = true;
          serialControl = true;
          sensor_off();

      }
    }

    if((unit == "deg" || unit == "degrees" || unit == "DEGREES") && (0 <= value && value <= 360)){
      // Assuming that the robot rotates from 0-360 degrees

      float period_calculation_angle = ((float)(value))/((6.0 * AVG_SPEED_RPM));

      period_angle = (unsigned long)(period_calculation_angle); 

      if (!rotating_angle)
      {
        // Start motors
        motorA.forward(DUTY_RATIO);
        motorB.reverse(DUTY_RATIO);

        prevTime_angle = millis(); // Start angle timer
        rotating_angle = true;
        serialControl = true;
        sensor_off();

      }

    }

  }


  // Check whether required running time has elapsed
  if (moving_distance == true && (millis() - prevTime_distance >= period_cm)) // Determines whether the current distance timer exceeds the required time
  {
      motorA.stop();
      motorB.stop();

      moving_distance = false;
  }


  // Check whether required running time has elapsed
  if (rotating_angle == true && (millis() - prevTime_angle >= period_angle)) //Determines whether the current distance timer exceeds the required time
  {
    motorA.stop();
    motorB.stop();

    rotating_angle = false;
  }

}

void turtle_motion(String command){

  if (command == "F" || command == "FORWARD")
    {
      serialControl = true;
      
      sensor_off();

      motorA.reverse(65);
      motorB.reverse(65);

    }
    else if (command == "B" || command == "BACKWARD")
    {
      serialControl = true;

      sensor_off();

      motorA.forward(65);
      motorB.forward(65);

    }
    else if (command == "L" || command == "LEFT")
    {
      serialControl = true;

      sensor_off();

      motorA.forward(65);
      motorB.reverse(65);

    }
    else if (command == "R" || command == "RIGHT")
    {
      serialControl = true;

      sensor_off();

      motorA.reverse(65);
      motorB.forward(65);

    }
    else if (command == "S" || command == "STOP")
    {
      serialControl = false;
      moving_distance = false;
      rotating_angle = false;

      sensor_on();
  
      motorA.stop();
      motorB.stop();
    }
}
*/