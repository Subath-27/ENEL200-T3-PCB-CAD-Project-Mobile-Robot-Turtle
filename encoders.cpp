#include "encoders.h"
#include <Arduino.h>

volatile unsigned long pulsesA;
volatile unsigned long pulsesB;

volatile unsigned long TIME = 0;

unsigned long TIME_PREV_A = 0;

unsigned long TIME_PREV_B = 0;


int E_PA;
int E_PB;

float AVG_SPEED_RPM;

float RPMA;
float RPMB;


unsigned int pulses_per_turn = 20;

#define CONVERSION_FACTOR 3600 // Converts from seconds -> minutes for the RPM Calculation

void encoders_init(){

  // Reset all to 0
  RPMA = 0;
  RPMB = 0;
  pulsesA = 0;
  pulsesB = 0;
  TIME = millis();


  pinMode(E_PA, INPUT);
  pinMode(E_PB, INPUT);

  // Connect the encoder's signals to external interrupts to count pulses on changes.
  attachInterrupt(digitalPinToInterrupt(E_PA), countPulsesA, RISING); 
  attachInterrupt(digitalPinToInterrupt(E_PB), countPulsesB, RISING);

}

void countPulsesA(){ // Counting the number of pulses for calculation of RPM (Motor Driver A)
  pulsesA++; 
}

void countPulsesB(){ // Counting the number of pulses for calculation of RPM (Motor Driver B)
  pulsesB++; 
}

float RPM_CalculationA(){

  unsigned long timeCurrent = millis();
  unsigned long DELTA_TIME = timeCurrent - TIME_PREV_A;

  if(DELTA_TIME > 100){

    noInterrupts();
    unsigned long pulseA = pulsesA;
    pulsesA = 0;
    interrupts();
    
    RPMA = (pulseA * (float)CONVERSION_FACTOR) / ((float)pulses_per_turn * DELTA_TIME);

    TIME_PREV_A = timeCurrent;

  }

  return RPMA;
}

float RPM_CalculationB(){

  unsigned long timeCurrent = millis();
  unsigned long DELTA_TIME = timeCurrent - TIME_PREV_B;

  if(DELTA_TIME > 100){

    noInterrupts();
    unsigned long pulseB = pulsesB;
    pulsesB = 0;
    interrupts();

    RPMB = (pulseB * (float)CONVERSION_FACTOR) / ((float)pulses_per_turn * DELTA_TIME);

    TIME_PREV_B = timeCurrent;

  }
  return RPMB;
}

float AVG_RPM_Calculation(float RPMA, float RPMB){

  AVG_SPEED_RPM = ((RPMA + RPMB)/2.0);
  return AVG_SPEED_RPM;

}


void print_RPMA() {
    Serial.print("RPM for Motor A: ");
    Serial.println(RPMA);
}

void print_RPMB() {
    Serial.print("RPM for Motor B: ");
    Serial.println(RPMB);
}


