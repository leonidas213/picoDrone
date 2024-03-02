#include "WolfLink.h"

void TelemetryFactory(void *constructorArr)
{
    WolfLink2 **arr = (WolfLink2 **)constructorArr;
    arr[0] = new Attitude();
    arr[1] = new Status();
    arr[2] = new Position();
    arr[3] = new ChannelsData();
}
void TestTelemetries()
{
    void *constructorarray[TotalPacketNum] = {0};
    TelemetryFactory(constructorarray);
    for (int i = 0; i < TotalPacketNum; i++)
    {
        
        WolfLink2 *temp = (WolfLink2 *)constructorarray[i];
        temp->test();
    }
}