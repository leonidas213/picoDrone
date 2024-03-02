
#include "pico/stdlib.h"
#include "global_inc.h"
#include "MPU6050.h"

void mpuReadInterrupt(uint gpio, uint32_t events);
void core1_entry();
extern double *yawPtr,* pitchPtr, *rollPtr;
class SensorController
{

private:
    //---------mpu6050--------100hz
    bool completed;
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    Quaternion q;      // [w, x, y, z]         quaternion container
    VectorInt aa;      // [x, y, z]            accel sensor measurements
    VectorInt gy;      // [x, y, z]            gyro sensor measurements
    VectorInt aaReal;  // [x, y, z]            gravity-free accel sensor measurements
    VectorInt aaWorld; // [x, y, z]            world-frame accel sensor measurements
    Vector3 gravity;   // [x, y, z]            gravity vector
    float euler[3];    // [psi, theta, phi]    Euler angle container
    float ypr[3];      // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

    MPU6050 mpu;

public:
    SensorController();
    void SetupMpu();
    Vector3 readAccel();
    Vector3 readGyro();
    Vector3 readMagnetic();
    Vector3 getYPR();
};