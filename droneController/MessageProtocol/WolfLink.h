#include "utilities.h"
#include "pico/stdlib.h"

/**
 * Best regards to Gökalp Akkurt
 */

void TelemetryFactory(void *constructorArr);
void TestTelemetries();
#define TotalPacketNum 4
#define GET_VARIABLE_NAME(Variable) (String(#Variable))
enum class ErrorCodes
{
    NO_ERROR,
    ERROR
};
enum class FlyModes
{
    STABILIZE,
    ALT_HOLD,
    LOITER,
    AUTO,
    RTL,
    LAND
};

enum class MessageType
{
    Telemetry = 1,
    Command,
    Telemetry_Command,

};

class WolfLink2
{
public:
    int MessageHertz = 0;
    int CommandMessageHertz = 0;

    virtual void Serialize(char *) = 0;
    virtual void Deserialize(char *) = 0;
    virtual void test()=0;
};

class Attitude : public WolfLink2
{

public:
    const uint16_t PacketNum = 1; // max 8.191
    const MessageType type = MessageType::Telemetry;
    uint16_t Header = 0;
#pragma pack(push, 1)
    typedef struct
    {
        uint16_t Header;
        float yaw, pitch, roll;
        float y_Acc, p_Acc, r_Acc;
        float heading;
    } ValuePacket;
#pragma pack(pop)
    uint32_t packetsize = sizeof(ValuePacket);
    ValuePacket packet;
    float yaw, pitch, roll;
    float y_Acc, p_Acc, r_Acc;
    float heading;
    Attitude()
    {
        MessageHertz = 20;
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));
    }
    virtual void Serialize(char *buffer) override
    {
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));

        packet = {
            Header,
            yaw,
            pitch,
            roll,
            y_Acc,
            p_Acc,
            r_Acc,
            heading};
        memcpy(buffer, &packet, packetsize);
    };
    virtual void Deserialize(char *message)
    {
        ValuePacket packet;
        memcpy(&packet, message, packetsize);

        yaw = packet.yaw;
        pitch = packet.pitch;
        roll = packet.roll;
        y_Acc = packet.y_Acc;
        p_Acc = packet.p_Acc;
        r_Acc = packet.r_Acc;
        heading = packet.heading;
    };

    virtual void test()
    {
        char messageBuffer[32];
        Attitude den = Attitude();
        den.yaw = 123;
        den.pitch = 0.2;
        den.heading = 358;
        den.p_Acc = -123;
        den.r_Acc = -85.8;
        den.Serialize(&messageBuffer[0]);
        SerialAndUsb.println("serialized");
        SerialAndUsb.print("packetsize: ");
        SerialAndUsb.println(den.packetsize);
        for (int i = 0; i < den.packetsize; i++)
        {
            int a = (int)(messageBuffer[i]);
            SerialAndUsb.print(a, HEX);
            SerialAndUsb.print(" ");
        }
        SerialAndUsb.println();
        Attitude yeniden = Attitude();
        yeniden.Deserialize(&messageBuffer[0]);

        SerialAndUsb.println("Deserialized");
        SerialAndUsb.print("Header: ");
        SerialAndUsb.println(yeniden.Header);
        SerialAndUsb.print("yaw: ");
        SerialAndUsb.print(yeniden.yaw);
        SerialAndUsb.print("  pitch: ");
        SerialAndUsb.print(yeniden.pitch);
        SerialAndUsb.print("  roll: ");
        SerialAndUsb.println(yeniden.roll);
        SerialAndUsb.print("y_Acc: ");
        SerialAndUsb.print(yeniden.y_Acc);
        SerialAndUsb.print("  p_Acc: ");
        SerialAndUsb.print(yeniden.p_Acc);
        SerialAndUsb.print("  r_Acc: ");
        SerialAndUsb.println(yeniden.r_Acc);
        SerialAndUsb.print("  heading: ");
        SerialAndUsb.println(yeniden.heading);
    }
    
};

class ChannelsData : public WolfLink2
{

public:
    const uint16_t PacketNum = 1; // max 8.191
    const MessageType type = MessageType::Telemetry_Command;
    uint16_t Header = 0;
#pragma pack(push, 1)
    typedef struct
    {
        uint16_t Header;
        uint32_t channel_1, channel_2, channel_3, channel_4;
        uint32_t channel_5, channel_6, channel_7;
        uint16_t buttons;
    } ValuePacket;
#pragma pack(pop)
    ValuePacket packet;
    uint32_t packetsize = sizeof(ValuePacket);
    uint32_t channel_1 = 0, channel_2 = 0, channel_3 = 0, channel_4 = 0;
    uint32_t channel_5 = 0, channel_6 = 0, channel_7 = 0;
    uint16_t buttons;
    ChannelsData()
    {
        MessageHertz = 10;
        CommandMessageHertz = 20;
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));
    }
    virtual void Serialize(char *buffer) override
    {
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));

        packet = {
            Header,
            channel_1, channel_2, channel_3, channel_4,
            channel_5, channel_6, channel_7, buttons

        };
        memcpy(buffer, &packet, packetsize);
    };
    virtual void Deserialize(char *message)
    {
        ValuePacket packet;
        memcpy(&packet, message, packetsize);
        channel_1 = packet.channel_1;
        channel_2 = packet.channel_2;
        channel_3 = packet.channel_3;
        channel_4 = packet.channel_4;
        channel_5 = packet.channel_5;
        channel_6 = packet.channel_6;
        channel_7 = packet.channel_7;
        buttons = packet.buttons;
    };
    void test()
    {
        char messageBuffer[32];
        ChannelsData den = ChannelsData();
        den.channel_1 = 1230;
        den.channel_2 = 2;
        den.channel_3 = 358;
        den.channel_4 = 1203;
        den.channel_5 = 2580;
        den.buttons = ((1 << 5) | (1 << 10) | (1 << 2) | (1 << 15));
        den.Serialize(&messageBuffer[0]);
        SerialAndUsb.println("serialized");
        SerialAndUsb.print("packetsize: ");
        SerialAndUsb.println(den.packetsize);
        for (int i = 0; i < den.packetsize; i++)
        {
            int a = (int)(messageBuffer[i]);
            SerialAndUsb.print(a, HEX);
            SerialAndUsb.print(" ");
        }
        SerialAndUsb.println();

        ChannelsData yeniden = ChannelsData();
        yeniden.Deserialize(&messageBuffer[0]);

        SerialAndUsb.println("Deserialized");
        SerialAndUsb.print("Header: ");
        SerialAndUsb.println(yeniden.Header);
        SerialAndUsb.print("channel_1: ");
        SerialAndUsb.print(yeniden.channel_1);
        SerialAndUsb.print("  channel_2: ");
        SerialAndUsb.print(yeniden.channel_2);
        SerialAndUsb.print("  channel_3: ");
        SerialAndUsb.println(yeniden.channel_3);
        SerialAndUsb.print(" channel_4: ");
        SerialAndUsb.print(yeniden.channel_4);
        SerialAndUsb.print("  channel_5: ");
        SerialAndUsb.print(yeniden.channel_5);
        SerialAndUsb.print("  channel_6: ");
        SerialAndUsb.println(yeniden.channel_6);
        SerialAndUsb.print("  channel_7: ");
        SerialAndUsb.println(yeniden.channel_7);
        SerialAndUsb.print("buttons:");
        SerialAndUsb.println(yeniden.buttons, BIN);
    }
};

class Status : public WolfLink2
{

public:
    const uint16_t PacketNum = 2; // max 8.191
    const MessageType type = MessageType::Telemetry;
    uint16_t Header = 0;
#pragma pack(push, 1)
    typedef struct
    {
        uint16_t Header;
        unsigned char battery;
        uint8_t VehicleStatus;
        uint8_t VehicleMode;
        uint8_t SensorStatus;
        uint16_t ErrorCode;
        float temperature;
        float humidity;
        float pressure;
        uint64_t unixTime;

    } ValuePacket;
#pragma pack(pop)
    ValuePacket packet;
    uint32_t packetsize = sizeof(ValuePacket);
    unsigned char battery = 0;
    uint8_t VehicleStatus = 0;
    uint8_t VehicleMode = 0;
    uint8_t SensorStatus = 0;
    uint16_t ErrorCode = 0;
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    uint64_t unixTime = 0;
    Status()
    {
        MessageHertz = 20;
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));
    }
    virtual void Serialize(char *buffer) override
    {
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));

        packet = {
            Header,
            battery,
            VehicleStatus,
            VehicleMode,
            SensorStatus,
            ErrorCode,
            temperature,
            humidity,
            pressure,
            unixTime};
        memcpy(buffer, &packet, packetsize);
    };
    virtual void Deserialize(char *message)
    {
        ValuePacket packet;
        memcpy(&packet, message, packetsize);
        Header = packet.Header;
        battery = packet.battery;
        VehicleStatus = packet.VehicleStatus;
        VehicleMode = packet.VehicleMode;
        SensorStatus = packet.SensorStatus;
        ErrorCode = packet.ErrorCode;
        temperature = packet.temperature;
        humidity = packet.humidity;
        pressure = packet.pressure;
        unixTime = packet.unixTime;
    };
    bool isArmed()
    {
        return VehicleMode & 0x01;
    }

    void test()
    {
        char messageBuffer[32];
        Status den = Status();
        den.battery = 87;
        den.VehicleStatus = (1 << 0) | (1 << 7);
        den.VehicleMode = (uint8_t)FlyModes::AUTO;
        den.SensorStatus = (1 << 0) | (1 << 3);
        den.ErrorCode = (int)ErrorCodes::ERROR;
        den.temperature = 32.45;
        den.humidity = 75.4;
        den.pressure = 10098.45;
        den.unixTime = time_us_64();

        den.Serialize(&messageBuffer[0]);
        SerialAndUsb.println("serialized");
        SerialAndUsb.print("packetsize: ");
        SerialAndUsb.println(den.packetsize);
        for (int i = 0; i < den.packetsize; i++)
        {
            int a = (int)(messageBuffer[i]);
            SerialAndUsb.print(a, HEX);
            SerialAndUsb.print(" ");
        }
        SerialAndUsb.println();

        Status yeniden = Status();
        yeniden.Deserialize(&messageBuffer[0]);

        SerialAndUsb.println("Deserialized");
        SerialAndUsb.print("Header: ");
        SerialAndUsb.println(yeniden.Header);
        SerialAndUsb.print("battery: ");
        SerialAndUsb.print(yeniden.battery);
        SerialAndUsb.print("  VehicleStatus: ");
        SerialAndUsb.print(yeniden.VehicleStatus, BIN);
        SerialAndUsb.print("  VehicleMode: ");
        SerialAndUsb.println(yeniden.VehicleMode);
        SerialAndUsb.print(" SensorStatus: ");
        SerialAndUsb.print(yeniden.SensorStatus, BIN);
        SerialAndUsb.print("  ErrorCode: ");
        SerialAndUsb.print(yeniden.ErrorCode);
        SerialAndUsb.print("  temperature: ");
        SerialAndUsb.println(yeniden.temperature);
        SerialAndUsb.print(" humidity: ");
        SerialAndUsb.println(yeniden.humidity);
        SerialAndUsb.print("pressure:");
        SerialAndUsb.println(yeniden.pressure);
        SerialAndUsb.print("unixTime:");
        SerialAndUsb.println(yeniden.unixTime);
        SerialAndUsb.println(time_us_64());
    }
};

class Position : public WolfLink2
{

public:
    const uint16_t PacketNum = 3; // max 8.191
    const MessageType type = MessageType::Telemetry;
    uint16_t Header = 0;
#pragma pack(push, 1)
    typedef struct
    {
        uint16_t Header;
        float local_x, local_y, local_z; // 4 byte per var
        double lattitude, longitude;     // 8 byte per var

    } ValuePacket;
#pragma pack(pop)
    uint32_t packetsize = sizeof(ValuePacket);
    ValuePacket packet;
    float local_x, local_y, local_z;
    double lattitude, longitude;
    Position()
    {
        MessageHertz = 20;
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));
    }
    virtual void Serialize(char *buffer) override
    {
        Header = (((uint16_t)type << 13) | (PacketNum & 0x1fff));

        packet = {
            Header,
            local_x,
            local_y,
            local_z,
            lattitude,
            longitude,
        };
        memcpy(buffer, &packet, packetsize);
    };
    virtual void Deserialize(char *message)
    {
        ValuePacket packet;
        memcpy(&packet, message, packetsize);

        local_x = packet.local_x;
        local_y = packet.local_y;
        local_z = packet.local_z;
        lattitude = packet.lattitude;
        longitude = packet.longitude;
    };

    void test()
    {
        char messageBuffer[32];
        Position den = Position();
        den.local_x = 17.5;
        den.local_y = 45.2;
        den.local_z = 789.45;
        den.lattitude = 158.14789652;
        den.longitude = -180.14784127;
        den.Serialize(&messageBuffer[0]);
        SerialAndUsb.println("serialized");
        SerialAndUsb.print("packetsize: ");
        SerialAndUsb.println(den.packetsize);
        for (int i = 0; i < den.packetsize; i++)
        {
            int a = (int)(messageBuffer[i]);
            SerialAndUsb.print(a, HEX);
            SerialAndUsb.print(" ");
        }
        SerialAndUsb.println();
        Position yeniden = Position();
        yeniden.Deserialize(&messageBuffer[0]);

        SerialAndUsb.println("Deserialized");
        SerialAndUsb.print("Header: ");
        SerialAndUsb.println(yeniden.Header);
        SerialAndUsb.print("local_x: ");
        SerialAndUsb.print(yeniden.local_x);
        SerialAndUsb.print("  local_y: ");
        SerialAndUsb.print(yeniden.local_y);
        SerialAndUsb.print("  local_z: ");
        SerialAndUsb.println(yeniden.local_z);
        SerialAndUsb.print("  lattitude: ");
        SerialAndUsb.print(yeniden.lattitude, 8);
        SerialAndUsb.print("  longitude: ");
        SerialAndUsb.println(yeniden.longitude, 8);
    }
};



 