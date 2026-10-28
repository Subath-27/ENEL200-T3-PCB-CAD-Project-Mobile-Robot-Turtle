#ifndef SENSOR_DETECTION
#define SENSOR_DETECTION

#include <HCSR04.h>

extern int TRIGGER_PIN;
extern int ECHO_PIN;
extern int BUZZER_PIN;

extern float DISTANCE_CM;

extern bool objectDetected;


// Initialise ultrasonic sensor and buzzer
void sensor_detection_init();


// Measure distance and update objectDetected
float sensor_measure_distance();


// Print the most recently measured distance
void sensor_print_distance();


// Enable/disable ultrasonic sensor
void sensor_on();
void sensor_off();

void buzzer_on();
void buzzer_off();

#endif