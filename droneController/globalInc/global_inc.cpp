#include"global_inc.h"

double roll, pitch, yaw, height;
double rollSt_Poi, pitchSt_Poi, yawSt_Poi, heightSt_Poi;
double rollPid_Out, pitchPid_Out, yawPid_Out, heightPid_Out;

PidSetup ROLL = PidSetup{
    .input = &roll,          // pointer
    .output = &rollPid_Out,  // pointer
    .setpoint = &rollSt_Poi, // pointer
    .kp = 1,
    .ki = 0.13,
    .kd = 5,
    .sampleTime = 10, // ms
    .outMin = 0,
    .outMax = 100,
    .controllerDirection = DIRECT,
    .pOn = P_ON_E,

    .aggresive = true,
    .aggroSetPoint = 5,
    .aggroKp = 10,
    .aggroKi = 1.3,
    .aggroKd = 50};

PidSetup PITCH = PidSetup{
    .input = &pitch,          // pointer
    .output = &pitchPid_Out,  // pointer
    .setpoint = &pitchSt_Poi, // pointer
    .kp = 1,
    .ki = 0.13,
    .kd = 5,
    .sampleTime = 10, // ms
    .outMin = 0,
    .outMax = 100,
    .controllerDirection = DIRECT,
    .pOn = P_ON_E,

    .aggresive = true,
    .aggroSetPoint = 5,
    .aggroKp = 10,
    .aggroKi = 1.3,
    .aggroKd = 50};

PidSetup YAW = PidSetup{
    .input = &yaw,          // pointer
    .output = &yawPid_Out,  // pointer
    .setpoint = &yawSt_Poi, // pointer
    .kp = 1,
    .ki = 0.13,
    .kd = 5,
    .sampleTime = 10, // ms
    .outMin = 0,
    .outMax = 100,
    .controllerDirection = DIRECT,
    .pOn = P_ON_E,

    .aggresive = true,
    .aggroSetPoint = 5,
    .aggroKp = 10,
    .aggroKi = 1.3,
    .aggroKd = 50};

PidSetup HEIGHT = PidSetup{
    .input = &height,          // pointer
    .output = &heightPid_Out,  // pointer
    .setpoint = &heightSt_Poi, // pointer
    .kp = 1,
    .ki = 0.13,
    .kd = 5,
    .sampleTime = 10, // ms
    .outMin = 0,
    .outMax = 100,
    .controllerDirection = DIRECT,
    .pOn = P_ON_E,

    .aggresive = true,
    .aggroSetPoint = 5,
    .aggroKp = 10,
    .aggroKi = 1.3,
    .aggroKd = 50};

// Global Variables