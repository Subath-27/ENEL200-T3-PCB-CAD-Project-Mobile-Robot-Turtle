
#ifndef MOTOR_DRIVERS_H
#define MOTOR_DRIVERS_H

#include <Arduino.h>
#include <UcTTDcMotor.h>
#include <UcMotorPWM.h>


typedef enum
{
    FORWARD_A,
    BRAKE_A,
    REVERSE_A,
    ROTATE_A,
    DRAWING_A
} MOTOR_STATES_A;


typedef enum
{
    FORWARD_B,
    BRAKE_B,
    REVERSE_B,
    ROTATE_B,
    DRAWING_B
} MOTOR_STATES_B;

typedef enum
{
  CIRCLE,
  TRIANGLE,
  SQUARE,
} SHAPES;


extern MOTOR_STATES_A stateA;
extern MOTOR_STATES_B stateB;


extern SHAPES current_shape;
extern bool draw_shape;
extern bool shapeInitialised;
extern bool allShapesCompleted;

void motor_init();


void FORWARD_DRIVE(
    MOTOR_STATES_A &stateA,
    MOTOR_STATES_B &stateB
);


void OBJECT_DETECTION(
    MOTOR_STATES_A &stateA,
    MOTOR_STATES_B &stateB
);

//void DRAW_SHAPES(SHAPES &current_shape, bool &draw_shape);

#endif

