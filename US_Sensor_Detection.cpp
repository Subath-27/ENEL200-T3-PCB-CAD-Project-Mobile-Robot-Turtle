#include "US_Sensor_Detection.h"

#include <Arduino.h>
#include <HCSR04.h>

//  *** NEED AN ACTIVE BUZZER NOT A PASSIVE BUZZER. THEY DON'T INTERFARE WITH THE MCU'S TIMERS

// ULTRASONIC SENSOR AND BUZZER PINS

//const int TRIGGER_PIN = 12;
//const int ECHO_PIN = 13;
//const int BUZZER_PIN = 7;



// SENSOR SETTINGS

#define MAX_SENSOR_DISTANCE 400.0
#define DESIRED_SENSOR_DISTANCE 35.0 // 50.0 100.0



// GLOBAL VARIABLES


float DISTANCE_CM = 0.0;

bool objectDetected = false;
bool buzzerTriggered = false;

int TRIGGER_PIN = 12;
int ECHO_PIN = 13;
int BUZZER_PIN = 7;



// ULTRASONIC SENSOR OBJECT


UltraSonicDistanceSensor distanceSensor(
    TRIGGER_PIN,
    ECHO_PIN
);

// SENSOR ON

void sensor_on()
{
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    //digitalWrite(TRIGGER_PIN, LOW);
}

void buzzer_on(){

    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(300);

}

void buzzer_off(){

    digitalWrite(BUZZER_PIN, LOW);
}


// SENSOR OFF

void sensor_off()
{
    /*
     * This does NOT electrically power off the HC-SR04.
     *
     * It simply prevents normal triggering by changing
     * the trigger pin to an INPUT.
     */

    pinMode(TRIGGER_PIN, INPUT);

    // Stop buzzer
    digitalWrite(BUZZER_PIN, LOW);

    // No object detected while sensor is disabled
    objectDetected = false;
}


// SENSOR INITIALISATION

void sensor_detection_init()
{
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(TRIGGER_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    objectDetected = false;
}


// MEASURE DISTANCE

float sensor_measure_distance()
{
    // Measure distance

    float distance_cm = distanceSensor.measureDistanceCm();
    DISTANCE_CM = distance_cm;


    // Invalid measurement

    if (distance_cm < 0)
    {
        //Serial.println("Invalid ultrasonic measurement");

        objectDetected = false;

        return distance_cm;
    }


    // Beyond maximum sensor range

    if (distance_cm > MAX_SENSOR_DISTANCE)
    {
        Serial.println("Object is outside sensor range");

        objectDetected = false;

        return distance_cm;
    }


    // Object detection

    if (distance_cm > 0 && distance_cm <= DESIRED_SENSOR_DISTANCE)
    {
        objectDetected = true;

        if (!buzzerTriggered)
        {
            Serial.println("Object detected");

            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(300);

            buzzerTriggered = true;
        }
    }

    else
    {
        objectDetected = false;

        buzzerTriggered = false;
    }

    return distance_cm; 


}
// PRINT DISTANCE

void sensor_print_distance()
{
    //Serial.print("Distance: ");
    //Serial.print(DISTANCE_CM);
    //Serial.println(" cm");
}

