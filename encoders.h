#ifndef ENCODERS

#define ENCODERS


extern int E_PA;
extern int E_PB;
extern float AVG_SPEED_RPM;

void encoders_init();

// Returns nothing and increments the volatile pulse;
void countPulsesA();
void countPulsesB();

float RPM_CalculationA();
float RPM_CalculationB();
float AVG_RPM_Calculation(float, float);

void print_RPMA();
void print_RPMB();
 

#endif