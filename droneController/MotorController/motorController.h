#include "stdint.h"
#include "utilities.h"
class MotorController
{

public:
    MotorController(int motorl1, int motorr1, int motorl2, int motorr2);
    void updateMotors(float s1, float s2, float s3, float s4);

private:
    uint16_t motor1;
    uint16_t motor2;
    uint16_t motor3;
    uint16_t motor4;
    int motorl1, motorl2, motorr1, motorr2;
};