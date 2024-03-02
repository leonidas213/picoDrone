
#ifndef _REDWINGS_H_
#define _REDWINGS_H_

#define Debug
#include "pico/stdlib.h"
#include "global_inc.h"
#include "PID_V1.h"
#include "motorController.h"
#include "utilities.h"
#include "WolfLink.h"
#include "SensorController.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

class RedWings
{
private:
    MotorController *motorController;
    ChannelsData channelData;
    PidController pid = PidController(PidMode::BASIC);
    SensorController *Sensors;
    FlyModes flyMode;
    bool armed;
    long long pidTimer = 0;

    float R_angle, P_angle, Y_angle, R_rate, P_rate, Y_rate;
    ErrorCodes errorCodesBuffer[15];
    int errorCodeCounter = 0;

public:
    RedWings();
    long timerDisarm;
    void initiliaze();
    void update();
    void setChannelData(ChannelsData *channelData);
    void setChannelData(String channelData);
    ChannelsData *getChannelData();
    String GetTelemetry();

    bool isArmed();
    ErrorCodes Arm();
    ErrorCodes Disarm();
    ErrorCodes Calibrate();
    ErrorCodes SetFlyMode(FlyModes);
    ErrorCodes SetFlyMode(String);
    FlyModes GetFlyMode();

    void onChannel1Change();
    void onChannel2Change();
    void onChannel3Change();
    void onChannel4Change();
    void onChannel5Change();
    void onChannel6Change();

    void onModeChange();
    //-----Checkfunctions-----
    void checkArm();
    void checkVehicleMode();


    //----Rtos functions----
    static void updateTask(void *pvParameters);
    static void telemetryTask(void *pvParameters);
    static void sensorTask(void *pvParameters);
    static void armTask(void *pvParameters);
    static void disarmTask(void *pvParameters);
    static void calibrateTask(void *pvParameters);
    static void writeLogTask(void *pvParameters);
};

#endif