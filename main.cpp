#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "droneController/Drone.h"

#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "utilities.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//***-----Objects-----***
RedWings drone = RedWings();
ChannelsData temp = ChannelsData();
//***-----MainCoreEntries-----***
void core1_Entry()
{

    drone.setChannelData(&temp);
    drone.initiliaze();
    while (true)
    {
        // SerialAndUsb.println("update\n");

        drone.update();
    }
}

int main()
{
    stdio_init_all();
    // initialize();

    // DEBUG_SerialAndUsb.println("startup Success\n");

    // vTaskStartScheduler();
    // panic_unsupported();

    void *constructorarray[TotalPacketNum] = {0};

    TelemetryFactory(constructorarray);
    SerialAndUsb.begin(0, 1, 115200);
    SerialAndUsb.wait();

    while (1)
    {
        TestTelemetries();
        sleep_ms(3000);
    };

    return 0;
}