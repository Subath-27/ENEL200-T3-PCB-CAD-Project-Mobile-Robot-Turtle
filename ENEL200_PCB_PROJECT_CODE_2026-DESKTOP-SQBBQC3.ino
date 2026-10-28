#include <Arduino.h>

#include "US_Sensor_Detection.h"
#include "motor_drivers.h"
#include "encoders.h"



void setup()
{
    Serial.begin(9600);

    sensor_detection_init();

    motor_init();

    stateA = FORWARD_A;
    stateB = FORWARD_B;

    Serial.println("System initialisation complete.");
    Serial.println("Robot starting in FORWARD state.");
}


void loop()
{
    if(draw_shape){
      DRAW_SHAPES(current_shape, draw_shape);
    }
  
    else if(stateA == FORWARD_A && stateB == FORWARD_B){
         FORWARD_DRIVE(stateA, stateB);
    }

    else{
        OBJECT_DETECTION(stateA, stateB);
    }
}




