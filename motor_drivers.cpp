// NEED AN FSM FOR THE FUNCTION
// --> void OBJECT_DETECTION(MOTOR_STATES_A &stateA, MOTOR_STATES_B &stateB)

#include "motor_drivers.h"
#include "US_Sensor_Detection.h"


#include <Arduino.h>
#include <UcTTDcMotor.h>
#include <UcMotorPWM.h>


// MOTOR DRIVER SETTINGS

// Motor Driver A
int DCA_i = 0;        // Initial Duty Cycle
int DCA_Max = 100;    // Maximum Duty Cycle

// Motor Driver B
int DCB_i = 0;        // Initial Duty Cycle
int DCB_Max = 100;    // Maximum Duty Cycle


// MOTOR OBJECTS

// Motor Driver A pins:
// D9 = forward
// D10 = reverse
UcTTDcMotor motorA(D10, D11); // Uses Timer 1  // 10 11

// Motor Driver B pins:
// D3 = forward
// D11 = reverse
UcTTDcMotor motorB(D3, D9); // Uses Timer 2 // 3 9

// Initially the robot starts in FORWARD
MOTOR_STATES_A stateA = FORWARD_A;
MOTOR_STATES_B stateB = FORWARD_B;


static unsigned long brakeTime = 0;
static unsigned long reverseTime = 0;
static unsigned long rotateTime = 0;
static unsigned long shapeStartTime = 0;

SHAPES current_shape = CIRCLE;
bool shapeInitialised = false;
bool draw_shape = false;
bool allShapesCompleted = false;


void motor_init()
{
    motorA.init();
    motorB.init();
}

void FORWARD_DRIVE(MOTOR_STATES_A &stateA, MOTOR_STATES_B &stateB) {    

    if (stateA == FORWARD_A && stateB == FORWARD_B)
    {

      static unsigned long lastSensorMeasure = 0;
      static unsigned long lastPrint = 0;

      sensor_on(); // Initize the ultra-sonic sensor to be fully on

      if (millis() - lastSensorMeasure >= 100) // Measure every 100ms
      {
          sensor_measure_distance();
          lastSensorMeasure = millis();
      }

      if (millis() - lastPrint >= 1000) // Print every 1s on the serial monitor
      {
          //sensor_print_distance();
          lastPrint = millis();
      }
      

      // The robot moves forward

      motorA.reverse(60); // 80
      motorB.reverse(60); // 80


      if (objectDetected)
      {

          motorA.stop();
          motorB.stop();

          stateA = BRAKE_A;
          stateB = BRAKE_B;

          brakeTime = millis();

          buzzer_off();

      }
    }

    }




void OBJECT_DETECTION(MOTOR_STATES_A &stateA, MOTOR_STATES_B &stateB)
{


    if (stateA == BRAKE_A && stateB == BRAKE_B)
    {
        // When an object's detected the ultra-sonic sensor are off. Eg. They are off while reversing
        sensor_off(); // Initize the ultra-sonic sensor to be fully on

        motorA.stop();
        motorB.stop();

        if (millis() - brakeTime >= 1500)
        {
            // Transition to REVERSE
            stateA = REVERSE_A;
            stateB = REVERSE_B;


            // Start the REVERSE timer
            reverseTime = millis();
        }
    }


    else if (stateA == REVERSE_A && stateB == REVERSE_B)
    {

        // Reverse both motors

        motorA.forward(50); // 25
        motorB.forward(50); // 25


        if (millis() - reverseTime >= 2500)
        {
            // Stop both motors before changing direction
            motorA.stop();
            motorB.stop();


            // Transition to ROTATE
            stateA = ROTATE_A;
            stateB = ROTATE_B;


            // Start the ROTATE timer
            rotateTime = millis();
        }
    }


    else if (stateA == ROTATE_A && stateB == ROTATE_B)
    {

        // Turn the Ultra-Sonic Sensor and the servo back on
        // For instance, to detect for objects while rotating
        // Ultra-Sonic Sensor back on

        static bool rotateInitialised = false;

        if(!rotateInitialised){

            sensor_on(); // Sensor back on
            rotateInitialised = true;
        }

        static unsigned long rotate_lastsensorMeasure = 0;
        static unsigned long rotate_lastPrint = 0;
        
        if(millis() - rotate_lastsensorMeasure >= 100){
            sensor_measure_distance();
            rotate_lastsensorMeasure = millis();
        }

        if (millis() - rotate_lastPrint >= 1000) // Print every 1s to the serial monitor
        {
            //sensor_print_distance();
            rotate_lastPrint = millis();
        }

        // ----------------------------------------------------
        // Differential drive
        //
        // Motor A -> FORWARD
        // Motor B -> REVERSE
        //
        // This rotates the robot on the spot.
        // ----------------------------------------------------

        motorA.forward(50);
        motorB.reverse(50);

        // Check for object while rotating

        if (objectDetected)
        {
            // Stop both motors
            motorA.stop();
            motorB.stop();


            // Transition back to BRAKE
            stateA = BRAKE_A;
            stateB = BRAKE_B;


            // Restart BRAKE timer
            brakeTime = millis();

            rotateInitialised = false;
            buzzer_off();
        }


        // If no object is detected:
        // Continue rotating for 5 seconds

        else if (millis() - rotateTime >= 5000)
        {
            motorA.stop();
            motorB.stop();

            //stateA = DRAWING_A;
            //stateB = DRAWING_B;

            stateA = FORWARD_A;
            stateB = FORWARD_B;

            sensor_on();

            rotateInitialised = false;

            current_shape = CIRCLE;
            draw_shape = true;
            allShapesCompleted = false;

            shapeStartTime = millis();

            Serial.println("ROTATE -> DRAWING");
        }
  }



  else if (stateA == DRAWING_A && stateB == DRAWING_B)
  {
      // Ultrasonic OFF while drawing
      sensor_off();

      // Buzzer OFF while drawing
      noTone(BUZZER_PIN);

      // Draw current shape
      //DRAW_SHAPES(current_shape, draw_shape);

      // Only leave DRAWING after ALL shapes are complete
      if (allShapesCompleted)
      {
          motorA.stop();
          motorB.stop();

          stateA = FORWARD_A;
          stateB = FORWARD_B;

          Serial.println("DRAWING -> FORWARD");
      }
  } 
}

/*void DRAW_SHAPES(SHAPES &current_shape, bool &draw_shape)
{
    static int shapeStep = 0;

    const int DRAW_SPEED = 50;

    const unsigned long SIDE_TIME = 600;

    const unsigned long CIRCLE_TIME = 1500;

    const unsigned long TRIANGLE_TURN_TIME = 700;

    const unsigned long SQUARE_TURN_TIME = 550;


    if(stateA == DRAWING_A && stateB == DRAWING_B)
    {

    switch(current_shape)
    {
      

      case CIRCLE:

      motorA.forward(70);
      motorB.forward(100);

      if (millis() - shapeStartTime >= CIRCLE_TIME)
      {
          motorA.stop();
          motorB.stop();

          current_shape = TRIANGLE;

          shapeStep = 0;
          shapeStartTime = millis();

          Serial.println("CIRCLE COMPLETE");
          Serial.println("CIRCLE -> TRIANGLE");
      }

      break;



  case TRIANGLE:

      switch(shapeStep)
      {
          case 0:     // First side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 1;
                  shapeStartTime = millis();
              }

              break;


          case 1:     // First 120 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= TRIANGLE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 2;
                  shapeStartTime = millis();
              }

              break;


          case 2:     // Second side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 3;
                  shapeStartTime = millis();
              }

              break;


          case 3:     // Second 120 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= TRIANGLE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 4;
                  shapeStartTime = millis();
              }

              break;


          case 4:     // Third side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 5;
                  shapeStartTime = millis();
              }

              break;


          case 5:     // Final 120 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= TRIANGLE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 6;
                  shapeStartTime = millis();
              }

              break;


          case 6:     // Triangle complete

              motorA.stop();
              motorB.stop();

              current_shape = SQUARE;

              shapeStep = 0;
              shapeStartTime = millis();

              Serial.println("TRIANGLE COMPLETE");
              Serial.println("TRIANGLE -> SQUARE");

              break;
      }

      break;



  case SQUARE:

      switch(shapeStep)
      {
          case 0:     // First side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 1;
                  shapeStartTime = millis();
              }

              break;


          case 1:     // First 90 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= SQUARE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 2;
                  shapeStartTime = millis();
              }

              break;


          case 2:     // Second side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 3;
                  shapeStartTime = millis();
              }

              break;


          case 3:     // Second 90 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= SQUARE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 4;
                  shapeStartTime = millis();
              }

              break;


          case 4:     // Third side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 5;
                  shapeStartTime = millis();
              }

              break;


          case 5:     // Third 90 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= SQUARE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 6;
                  shapeStartTime = millis();
              }

              break;


          case 6:     // Fourth side

              motorA.forward(DRAW_SPEED);
              motorB.forward(DRAW_SPEED);

              if(millis() - shapeStartTime >= SIDE_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 7;
                  shapeStartTime = millis();
              }

              break;


          case 7:     // Final 90 degree turn

              motorA.forward(DRAW_SPEED);
              motorB.reverse(DRAW_SPEED);

              if(millis() - shapeStartTime >= SQUARE_TURN_TIME)
              {
                  motorA.stop();
                  motorB.stop();

                  shapeStep = 8;
                  shapeStartTime = millis();
              }

              break;


          case 8:     // Square complete

              motorA.stop();
              motorB.stop();

              shapeStep = 0;
              allShapesCompleted = true;

              Serial.println("SQUARE COMPLETE");

              break;
      }

      break;
  }
  }
}
*/
