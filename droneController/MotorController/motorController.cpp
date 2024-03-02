#include "motorController.h"
MotorController::MotorController(int motorl1, int motorr1, int motorl2, int motorr2)
{

    analogWrite(motorl1, 0);
    analogWrite(motorl2, 0);
    analogWrite(motorr1, 0);
    analogWrite(motorr2, 0);
}
void MotorController::updateMotors(float s1, float s2, float s3, float s4)
{   /*
    Front left motor = s1
    Rear left motor = s2
    Front right motor = s3
    Rear right motor = s4
*/
    analogWrite(motorl1, s1);
    analogWrite(motorl2, s2);
    analogWrite(motorr1, s3);
    analogWrite(motorr2, s4);

}