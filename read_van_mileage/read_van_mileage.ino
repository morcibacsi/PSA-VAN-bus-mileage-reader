#include <Arduino.h>
#include <SPI.h>
#include <itss46x.h>
#include <tss46x_van.h>

#include <tss463.h>

const int SCK_PIN = 25;
const int MISO_PIN = 5;
const int MOSI_PIN = 33;
const int VAN_CS_PIN = 32; //ESP32

#ifdef USE_BLUETOOTH_SERIAL
    #include <BluetoothSerial.h>
    #include "src/SerialPort/BluetoothSerialAbs.h"
    BluetoothSerial SerialBT;
#else
    #include "src/SerialPort/HardwareSerialAbs.h"
#endif

TaskHandle_t VANReadDataTask;
TaskHandle_t VANWriteDataTask;
SPIClass* spi;
ITss46x* vanSender;
TSS46X_VAN* VANInterface;

#include <esp32_arduino_rmt_van_rx.h>
#include <esp32_rmt_van_rx.h>

ESP32_RMT_VAN_RX VAN_RX;

const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;
const uint8_t VAN_DATA_RX_PIN = 21;
const uint8_t VAN_DATA_RX_LED_INDICATOR_PIN = 2;

uint8_t vanMessageLength;
uint8_t vanMessage[34];

uint32_t lastMillis = 0;

union UInt24
{
    struct
    {
        uint8_t rightByte;
        uint8_t middleByte;
        uint8_t leftByte;
    } data;
    uint32_t asUint24;
};

void QueryInstrumentClusterForMileage(uint8_t channelId)
{
    VANInterface->set_channel_for_reply_request_message(channelId, 0x8FC, 7, 1);
}

void VANReadDataTaskFunction(void * parameter)
{
    VAN_RX.Init(VAN_DATA_RX_RMT_CHANNEL, VAN_DATA_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_COMFORT);
    //VAN_RX.Init(VAN_DATA_RX_RMT_CHANNEL, VAN_DATA_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_BODY);

    for (;;)
    {
        VAN_RX.Receive(&vanMessageLength, vanMessage);

        if (vanMessageLength > 0)
        {
            if(VAN_RX.IsCrcOk(vanMessage, vanMessageLength))
            {
                for (size_t i = 0; i < vanMessageLength; i++)
                {
                    if (i != vanMessageLength - 1)
                    {
                        printf("%02X ", vanMessage[i]);
                    }
                    else
                    {
                        printf("%02X", vanMessage[i]);
                    }
                }
                printf("\n");

                if (vanMessage[1] == 0x8F && vanMessage[2] == 0xCE)
                {
                    UInt24 odo;
                    odo.data.leftByte   = vanMessage[4];
                    odo.data.middleByte = vanMessage[5];
                    odo.data.rightByte  = vanMessage[6];
                    Serial.print("Mileage: ");
                    Serial.print(odo.asUint24 / 10.0);
                    Serial.println(" km");
                }
            }
            else
            {
                printf("CRC ERROR!\n");
            }
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void VANWriteDataTaskFunction(void * parameter)
{
    spi = new SPIClass();
    spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_CS_PIN);

    vanSender = new Tss463(VAN_CS_PIN, spi);
    VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
    VANInterface->begin();

    for (;;)
    {
        VANInterface->set_channel_for_reply_request_message(2, 0x8FC, 7, 1);

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(500000);
    printf("PSA VAN bus mileage reader\n");

    xTaskCreatePinnedToCore(
        VANReadDataTaskFunction,        // Function to implement the task
        "VANReadDataTask",              // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        1,                              // Priority of the task (higher the number, higher the priority)
        &VANReadDataTask,               // Task handle.
        0);                             // Core where the task should run

    xTaskCreatePinnedToCore(
        VANWriteDataTaskFunction,        // Function to implement the task
        "VANWriteDataTask",              // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        1,                              // Priority of the task (higher the number, higher the priority)
        &VANWriteDataTask,               // Task handle.
        1);                             // Core where the task should run
}

void loop()
{
    vTaskDelay(50 / portTICK_PERIOD_MS);
}