#ifndef GLOBAL_INC_H
#define GLOBAL_INC_H
#include "PID_v1.h"
// Battery Voltage measurment
#define BATV_OFFSET 0.195 // Battery voltage offset due to ADC non liniarity and Voltage divider resistor tolerance
#define NORMAL_BAT_V 4.2  // Normal Battery Voltage as init value

#define MOTOR_L1 3 // MA1 Front Left
#define MOTOR_L2 2 // MA3 Rear  Left
#define MOTOR_R1 0 // MB1 Front Right
#define MOTOR_R2 1 // MB3 Rear  Right

#define MOT_IDLE 60 // Idle speed of motor armed and throttle is minimum

// I2C PIN Defination for Wire0 to Interface MPU6050 IMU
#define SDA_PIN 16          // SDA PIN
#define SCL_PIN 17          // SCL PIN
#define I2C_CLK_FREQ 400000 // Fast I2C 400kHz Clk

// settings
#define PID_LOOP_US 8000 // PID loop interval in us
#define ARM_BUTTON 1
extern double rollSt_Poi, pitchSt_Poi, yawSt_Poi, heightSt_Poi;
extern double rollPid_Out, pitchPid_Out, yawPid_Out, heightPid_Out;

extern PidSetup ROLL, PITCH, YAW, HEIGHT;

// Global Variables
extern double rollacc, pitchacc, yawacc;
extern double roll, pitch, yaw, heading;
extern double height;

#endif
