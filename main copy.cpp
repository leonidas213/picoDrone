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
#define PRINT_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define SENSOR_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define TELEMETRY_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define DRONE_UPDATE_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define ARM_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define DISARM_TASK_PRIORITY (tskIDLE_PRIORITY + 10)
#define CALIBRATE_TASK_PRIORITY (tskIDLE_PRIORITY + 10)

//***-----Tasks------***
TaskHandle_t tassk1;
TaskHandle_t PrintTask;
TaskHandle_t DroneUpdateTask;
TaskHandle_t DroneTelemtryTask;
TaskHandle_t DroneSensorTask;
TaskHandle_t DroneArmTask;
TaskHandle_t DroneDisarmTask;
TaskHandle_t DroneCalibrateTask;

//***-----Queues-----***
QueueHandle_t PrintQue0ue;

//***-----Mutexes------***
// mutex_enter_blocking(&xSDKMutex);
// mutex_exit(&xSDKMutex);
auto_init_mutex(xSDKMutex);

//***-----Semaphores------***
// sem_init(&xSDKSemaphore, 0, 1);
// sem_acquire_timeout_us(&xSDKSemaphore, 250500)
// sem_release(&xSDKSemaphore);
static semaphore_t xSDKSemaphore;

//***-----Objects-----***
RedWings drone = RedWings();
ChannelsData temp = ChannelsData();

//***-----HelperFuncs-----***
bool reserved_addr(uint8_t addr)
{

    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
int busScan(i2c_inst *i2c)
{
    // Enable UART so we can print status output

    // This example will use i2c on the default SDA and SCL pins (GP4, GP5 on a Pico)

    // Make the I2C pins available to picotool
    // bi_decl(bi_2pins_with_func(2, 3, GPIO_FUNC_I2C));

    SerialAndUsb.println("\nI2C Bus Scan\n");
    SerialAndUsb.println("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr)
    {
        if (addr % 16 == 0)
        {
            SerialAndUsb.println(addr, HEX);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;

        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c, addr, &rxdata, 1, false);

        SerialAndUsb.println(ret < 0 ? "." : "@");
        SerialAndUsb.println(addr % 16 == 15 ? "\n" : "  ");
    }
    SerialAndUsb.println("Done.\n");
    return 0;
}

//***-----Tasks-----***
void periodicTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS(500);
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
        SerialAndUsb.println("periodicTask\n");
    }
}
void SerialPrintTask(void *pvParameters)
{
    char *message;
    while (1)
    {
        if (xQueueReceive(PrintQueue, &message, portMAX_DELAY))
        {

            Serial.println(message);
        }
    }
}
//***-----Initializers-----***
void initializeInterfaces()
{
    gpio_init(6);
    gpio_init(7);
    gpio_set_function(6, GPIO_FUNC_I2C);
    gpio_set_function(7, GPIO_FUNC_I2C);
    i2c_init(i2c1, 200 * 1000);
    Serial.begin(0, 1, 115200, uart0);
}
void initializeSemaphores()
{
}
void initializeQueues()
{
    PrintQueue = xQueueCreate(10, sizeof(char *));
}
void initializeTasks()
{

    uint32_t status = xTaskCreate(
        SerialPrintTask,
        "Printer",
        configMINIMAL_STACK_SIZE,
        NULL,
        PRINT_TASK_PRIORITY,
        &PrintTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create Printer task");
    }
    status = xTaskCreate(
        drone.updateTask,
        "DroneUpdateTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        DRONE_UPDATE_TASK_PRIORITY,
        &DroneUpdateTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneUpdateTask task");
    }
    status = xTaskCreate(
        drone.telemetryTask,
        "DroneTelemtryTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        TELEMETRY_TASK_PRIORITY,
        &DroneTelemtryTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneTelemtryTask task");
    }
    status = xTaskCreate(
        drone.sensorTask,
        "DroneSensorTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        SENSOR_TASK_PRIORITY,
        &DroneSensorTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneSensorTask task");
    }

    status = xTaskCreate(
        drone.armTask,
        "DroneArmTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        ARM_TASK_PRIORITY,
        &DroneArmTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneArmTask task");
    }
    status = xTaskCreate(
        drone.disarmTask,
        "DroneDisarmTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        DISARM_TASK_PRIORITY,
        &DroneDisarmTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneDisarmTask task");
    }

    status = xTaskCreate(
        drone.calibrateTask,
        "DroneCalibrateTask",
        configMINIMAL_STACK_SIZE,
        NULL,
        CALIBRATE_TASK_PRIORITY,
        &DroneCalibrateTask);
    if (status != pdPASS)
    {
        SerialAndUsb.println("Failed to create DroneCalibrateTask task");
    }
}
void initializeTimers()
{
    // xTimerCreate("defTimer", pdMS_TO_TICKS(100), pdTRUE, (void *)0, periodicTaskCallback);
}
void initialize()
{
    initializeInterfaces();
    initializeSemaphores();
    initializeQueues();
    initializeTasks();
}
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

    void *constructorarray[4] = {0};

    TelemetryFactory(constructorarray);
    SerialAndUsb.begin(0, 1, 115200);
    SerialAndUsb.wait();
    Attitude dem3 = Attitude();
    Status dem2 = Status();
    Position dem = Position();
    ChannelsData dem1 = ChannelsData();
    while (1)
    {
        dem.test();
        dem1.test();
        dem2.test();
        dem3.test();
        sleep_ms(3000);
    };

    return 0;
}
/*

uint32_t ulVariable;

 //Write the 32-bit 0x12345678 value directly into index 1 of the thread
 //local storage array.  Passing NULL as the task handle has the effect of writing
 //to the calling task's thread local storage array.
vTaskSetThreadLocalStoragePointer(NULL,  //Task handle.
                                  1,     //Index into the array.
                                  (void *)0x12345678);
 //Store the value of the 32-bit variable ulVariable to index 0 of the calling
 //task's thread local storage array.
ulVariable = ERROR_CODE;
vTaskSetThreadLocalStoragePointer(NULL,  //Task handle.
                                  0,     //Index into the array.
                                  (void *)ulVariable);

 //Read the value stored in index 5 of the calling task's thread local storage
 //array into ulVariable.
ulVariable = (uint32_t)pvTaskGetThreadLocalStoragePointer(NULL, 5);

//Storing and retrieving 32 - bit values directly from an index in the thread local storage array

                                    */
