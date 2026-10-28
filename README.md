# ENEL200-T3-PCB-CAD-Project-Mobile-Robot-Turtle

Full Arduino code for the Mobile Robot Turtle Project (ENEL200, University of Canterbury, 2026).

<img width="300" alt="Finished Mobile Robot Turtle" src="https://github.com/user-attachments/assets/b5dc2c1b-1ded-4c74-b9b6-ad6027468058" />

## Overview
The purpose of this project was to design and implement a mobile turtle robot that helps children learn geometry and programming concepts in a logical and entertaining way, using serial Logo commands.

## Features
- Drives and turns in response to Logo commands sent over the serial interface
- Ultrasonic sensor detects obstructions greater than 0 cm and up to 30 cm away
- Active buzzer sounds whenever an obstruction is detected
- Custom PCB shield designed in KiCad to drive the two DC motors and connect the buzzer and ultrasonic sensor

## Hardware
- Board: Arduino Uno
- Motors: 2 × DC gearbox (TT) motors, 200 RPM (3-6 V DC)
- Sensors: ultrasonic sensor
- Buzzer: passive
- Power: 6 × AA alkaline batteries (9 V pack) with a rocker on/off switch

## Software
- Arduino C++, Arduino IDE 2.3.4
- Libraries: `HCSR04.h`, `Arduino.h`, plus the university-provided `UcTTDcMotor.h` and `UcMotorPWM.h` for motor control

## Results
- Responded to Logo commands delivered through the serial interface
- Carried a pen and drew on the surface it moved over
- Drove forward in reasonably straight lines
- Rotated on the spot in increments of 10 degrees or less
- Gave visual feedback on its actions
- Ran from a portable battery pack with no tether

Grade: 4.7 / 6 (78%)

## How to build and upload
1. Open the `.ino` file in Arduino IDE.
2. Install any libraries listed above.
3. Select your board and port, then click Upload.

## Author
Subath Heenkenda Mudiyanselage | [LinkedIn](https://www.linkedin.com/in/subath-heenkenda27/) | subath.heenkenda@outlook.com

Team: Benjamin Murphy helped solder the PCB shield.
