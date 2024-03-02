

#include "Drone.h"
RedWings::RedWings()
{
    
}

void RedWings::initiliaze()
{
    pid.CreatePid(&YAW, PidType::YAW);
    pid.CreatePid(&PITCH, PidType::PITCH);
    pid.CreatePid(&ROLL, PidType::ROLL);
    pid.CreatePid(&HEIGHT, PidType::HEIGHT);
    motorController = new MotorController(MOTOR_L1, MOTOR_R1, MOTOR_L2, MOTOR_R2);

    // mpu.init_MPU6050();
    printf("calibrating");
    // mpu.compute_OFFSET(); // for production version
    // mpu.compAngleX = 0;
    // mpu.compAngleY = 0;
    // mpu.compAngleZ = 0;
    R_angle = 0;
    P_angle = 0;
    Y_angle = 0;
    // pid->reset();
    timerDisarm = time_us_32();
}
void RedWings::update()
{ // Gyro readings in angle and rate
    /*float R_in = 0, P_in = 0, Y_in = 0, R_set, P_set, Y_set;                         // PID inputs and set points
    uint16_t T_ch = 0, R_ch = 500, P_ch = 500, Y_ch = 500, Aux_ch1 = 0, Aux_ch2 = 0; // Ibus RX 6 Channels
    float roll_level_adjust, pitch_level_adjust;                                     // PID less Rate adjust
    uint32_t Timer_pid = 0;
    float Speed1 = 0, Speed2 = 0, Speed3 = 0, Speed4 = 0, Speed5 = 0, Speed6 = 0; // Motor Speed
    uint32_t arm_flash_count = 0, just_armed = 0, main_loop_count = 0;            // other Vars
    bool isCalibrated = false;*/

    checkArm();
    if (time_us_64() - pidTimer > PID_LOOP_US)
    {
        pidTimer = time_us_64();
        /*
        // mpu.compute_RPY(R_angle, P_angle, Y_angle, R_rate, P_rate, Y_rate);

        pitch_level_adjust = P_angle * 13;
        roll_level_adjust = R_angle * 13;

        // Update set point with effect of angle for Auto level mode
        R_set = -((float)R_ch - 1500) * 6.6;
        P_set = ((float)P_ch - 1500) * 6.6;
        R_set -= roll_level_adjust;
        P_set += pitch_level_adjust;
        R_set = R_set / 8.5;
        P_set = P_set / 8.5;
        Y_set = ((float)Y_ch - 1500) / 8.0;
        R_in = (R_in * 0.90) + (R_rate * 0.10); // Gyro pid R input is deg/sec.
        P_in = (P_in * 0.90) + (P_rate * 0.10); // Gyro pid P input is deg/sec.
        Y_in = (Y_in * 0.90) + (Y_rate * 0.10); // Gyro pid Y input is deg/sec.

        Vector3 tv = Vector3(Y_set, P_set, R_set);

        // pid->setTargetYPR(tv);
        tv = Vector3(Y_in, P_in, R_in);
        // pid->setYPR(tv);
        // PIDPacket packet = pid->updatePID();
        // Speed1 = ((float)T_ch - 1000 + MOT_IDLE) - packet.pitch + packet.roll + packet.yaw; // Calculate the pulse for esc 1 (front-left  - CCW)
        // Speed2 = ((float)T_ch - 1000 + MOT_IDLE) + packet.pitch + packet.roll - packet.yaw; // Calculate the pulse for esc 2 (rear-left   - CW )
        // Speed3 = ((float)T_ch - 1000 + MOT_IDLE) + packet.pitch + packet.roll + packet.yaw; // Calculate the pulse for esc 3 (rear-right  - CCW)
        // Speed4 = ((float)T_ch - 1000 + MOT_IDLE) - packet.pitch - packet.roll - packet.yaw; // Calculate the pulse for esc 4 (front-right - CW )

        if (Speed1 >= 900)
            Speed1 = 900;
        if (Speed2 >= 900)
            Speed2 = 900;
        if (Speed3 >= 900)
            Speed3 = 900;
        if (Speed4 >= 900)
            Speed4 = 900;
        if (Speed1 <= 0)
            Speed1 = 0;
        if (Speed2 <= 0)
            Speed2 = 0;
        if (Speed3 <= 0)
            Speed3 = 0;
        if (Speed4 <= 0)
            Speed4 = 0;

        if (channelData.channel_1 > 1050)
        {
            motorController->updateMotors(Speed1, Speed2, Speed3, Speed4);
        }
        else
        {
            motorController->updateMotors(MOT_IDLE, MOT_IDLE, MOT_IDLE, MOT_IDLE);
            // pid->reset();
        }*/
    }

    /*if (channelData.channel_6 > 1800)
    {
        Arm();
    }
    else
    {
        // printf("disarm");
        if (armed)
        {
            Disarm();
        }
        isCalibrated = false;
        motorController->updateMotors(0, 0, 0, 0);
        // pid->reset();
    }*/
}

void RedWings::setChannelData(ChannelsData *channelData)
{
    this->channelData.channel_1 = channelData->channel_1;
    this->channelData.channel_2 = channelData->channel_2;
    this->channelData.channel_3 = channelData->channel_3;
    this->channelData.channel_4 = channelData->channel_4;
    this->channelData.channel_5 = channelData->channel_5;
    this->channelData.channel_6 = channelData->channel_6;
    this->channelData.channel_7 = channelData->channel_7;
    this->channelData.buttons = channelData->buttons;
}

ChannelsData *RedWings::getChannelData()
{
    return &channelData;
}

String RedWings::GetTelemetry()
{

    return "";
}

bool RedWings::isArmed()
{
    return armed;
}

ErrorCodes RedWings::Arm()
{
    // printf("Armed\n");
    armed = true;
    return ErrorCodes::NO_ERROR;
}

ErrorCodes RedWings::Disarm()
{
    armed = false;
    printf("Disarmed\n");
    return ErrorCodes::NO_ERROR;
}

ErrorCodes RedWings::Calibrate()
{
    return ErrorCodes::NO_ERROR;
}

ErrorCodes RedWings::SetFlyMode(FlyModes flyMode)
{
    this->flyMode = flyMode;
    return ErrorCodes::NO_ERROR;
}

ErrorCodes RedWings::SetFlyMode(String flyMode)
{
    flyMode.toLowerCase();
    if (flyMode == "stabilize")
        SetFlyMode(FlyModes::STABILIZE);
    else if (flyMode == "alt_hold")
        SetFlyMode(FlyModes::ALT_HOLD);
    else if (flyMode == "loiter")
        SetFlyMode(FlyModes::LOITER);
    else if (flyMode == "auto")
        SetFlyMode(FlyModes::AUTO);
    else if (flyMode == "rtl")
        SetFlyMode(FlyModes::RTL);
    else if (flyMode == "land")
        SetFlyMode(FlyModes::LAND);

    return ErrorCodes::NO_ERROR;
}

FlyModes RedWings::GetFlyMode()
{
    return flyMode;
}

void RedWings::onChannel1Change()
{
    // printf("Channel 1: %d\n", this->channelData.channel_1);
}

void RedWings::onChannel2Change()
{
    // printf("Channel 2: %d\n", this->channelData.channel_2);
}

void RedWings::onChannel3Change()
{
    // printf("Channel 3: %d\n", this->channelData.channel_3);
}

void RedWings::onChannel4Change()
{
    // printf("Channel 4: %d\n", this->channelData.channel_4);
}

void RedWings::onChannel5Change()
{
    // printf("Channel 5: %d\n", this->channelData.channel_5);
}

void RedWings::onChannel6Change()
{
    // printf("Channel 6: %d\n", this->channelData.channel_6);
}

void RedWings::onModeChange()
{
    // printf("Mode: %d\n", this->channelData.channel_6);
}
//------------CHECKS------------------------
void RedWings::checkArm()
{
    if ((this->channelData.buttons && ARM_BUTTON) && !isArmed())
    {
        ErrorCodes erc = Arm();
        switch (erc)
        {
        case ErrorCodes::NO_ERROR:
            break;

        default:
            errorCodesBuffer[errorCodeCounter] = erc;
            errorCodeCounter++;
            break;
        }
    }
    else
    {
        ErrorCodes erc = Disarm();
        switch (erc)
        {
        case ErrorCodes::NO_ERROR:
            break;

        default:
            errorCodesBuffer[errorCodeCounter] = erc;
            errorCodeCounter++;
            break;
        }
    }
}
//------------TASKS-------------------------
void RedWings::updateTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
    while (1)
    {
        drone->update();
        vTaskDelay(1);
    }
}

void RedWings::telemetryTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
}

void RedWings::sensorTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
    while (1)
    {
        // drone->mpu.update();
        vTaskDelay(1);
    }
}

void RedWings::armTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
    while (1)
    {
        // drone->Arm();
        vTaskDelay(1);
    }
}

void RedWings::disarmTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
    while (1)
    {
        // drone->Disarm();
        vTaskDelay(1);
    }
}

void RedWings::calibrateTask(void *pvParameters)
{
    RedWings *drone = (RedWings *)pvParameters;
    while (1)
    {
        // drone->Calibrate();
        vTaskDelay(1);
    }
}

void RedWings::writeLogTask(void *pvParameters)
{
    QueueHandle_t *LogQueue = (QueueHandle_t *)pvParameters;
    String LogMessage;
    while (1)
    {
        if (xQueueReceive(*LogQueue, &LogMessage, portMAX_DELAY) == pdTRUE)
        {
            Serial.println(LogMessage);
        }
        vTaskDelay(1);
    }
}
//--------------TelemetryTasks--------------
void attitudeTelemtry(void *pvParams)
{
    Attitude *attitude = (Attitude *)pvParams;
    float workTime = 1 / attitude->MessageHertz;
    while (1)
    {
        attitude->yaw = yaw;
        attitude->pitch = pitch;
        attitude->roll = roll;

        attitude->y_Acc = yawacc;
        attitude->p_Acc = pitchacc;
        attitude->r_Acc = rollacc;
        attitude->heading = heading;
        vTaskDelay(workTime);
    }
}