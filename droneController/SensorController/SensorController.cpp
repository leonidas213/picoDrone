#include "SensorController.h"
double *yawPtr, *pitchPtr, *rollPtr;

int mpuCounter = 0;
bool mpuInterrupt = false;
void mpuReadInterrupt(uint gpio, uint32_t events)
{
    mpuCounter++;
    if (mpuCounter >= 2)
    {
        mpuCounter = 0;
        mpuInterrupt = true;
    }
}

void SensorController::SetupMpu()
{

    printf("starting mpu\n");
    mpu.initialize(i2c0);
    devStatus = mpu.dmpInitialize();

    printf("calibrating mpu\n");
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    if (devStatus == 0)
    {
        mpu.setDMPEnabled(true); // turn on the DMP, now that it's ready
        // mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;                         // set our DMP Ready flag so the main loop() function knows it's okay to use it
        packetSize = mpu.dmpGetFIFOPacketSize(); // get expected DMP packet size for later comparison
        printf("success\n");
        completed = true;
        gpio_set_irq_enabled_with_callback(8, GPIO_IRQ_EDGE_FALL, true, &mpuReadInterrupt);
    }
    else
    {
        // ERROR!        1 = initial memory load failed         2 = DMP configuration updates failed        (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)", devStatus);
        sleep_ms(2000);
    }
    //---------mpu6050--------
}
Vector3 SensorController::getYPR()
{

    //---------mpu6050--------

    if (dmpReady)
    // if programming failed, don't try to do anything
    {
        if (mpuInterrupt)
        {

            mpuIntStatus = mpu.getIntStatus();
            mpuInterrupt = false;
            fifoCount = mpu.getFIFOCount(); // get current FIFO count
            // printing mpuIntStatus in hex

            if ((mpuIntStatus & 0x10) || fifoCount == 1024) // check for overflow (this should never happen unless our code is too inefficient)
            {
                mpu.resetFIFO(); // reset so we can continue cleanly
                // printf("FIFO overflow!");
            }
            else if (mpuIntStatus & 0x01) // otherwise, check for DMP data ready interrupt (this should happen frequently)
            {

                while (fifoCount < packetSize)
                    fifoCount = mpu.getFIFOCount();       // wait for correct available data length, should be a VERY short wait
                mpu.getFIFOBytes(fifoBuffer, packetSize); // read a packet from FIFO

                fifoCount -= packetSize; // track FIFO count here in case there is > 1 packet available
                                         // display Euler angles in degrees
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                *yawPtr = (ypr[0] * 180 / PI) > 0 ? (ypr[0] * 180 / PI) : 360 + (ypr[0] * 180 / PI);
                // yaw = (ypr[0] * 180 / PI);
                *pitchPtr = (ypr[1] * 180 / PI) > 0 ? (ypr[1] * 180 / PI) : 360 + (ypr[1] * 180 / PI);
                *rollPtr = (ypr[2] * 180 / PI) > 0 ? (ypr[2] * 180 / PI) : 360 + (ypr[2] * 180 / PI);

                if (*yawPtr > 360)
                    *yawPtr -= 360;
                if (*pitchPtr > 360)
                    *pitchPtr -= 360;
                if (*rollPtr > 360)
                    *rollPtr -= 360;

                // display real acceleration, adjusted to remove gravity
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                // printf("areal: %d,\t %d,\t %d\n", aaReal.x, aaReal.y, aaReal.z);
            }
        }
    }
}
