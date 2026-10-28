#ifndef SERIAL_COMMAND
#define SERIAL_COMMAND

#include <Arduino.h>
#include <UcTTDcMotor.h>
#include <UcMotorPWM.h>

unsigned long period_calculation_cm(int cm);
unsigned long period_calculation_angle(int deg);

extern String command;


extern bool serialControl;

void serial_commands();
void turtle_motion(String action, int value);

#endif