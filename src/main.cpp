#include <Arduino.h>
#include <NimBLEDevice.h>
#include "remote_scales.h"
#include "scales/acaia.h"
#include "soc/rtc.h"

typedef enum {
    SCAN_FOR_SCALES = 0,
    SCALE_FOUND     = 1,
    SCALE_CONNECTED = 2

} PROGRAM_STATE;

RemoteScalesScanner scanner;
std::unique_ptr<RemoteScales> remote_scale;
uint8_t state = PROGRAM_STATE::SCAN_FOR_SCALES;

void setup()
{
    Serial.begin(115200);

    while (!Serial);

    // rtc_cpu_freq_config_t config;
    // rtc_clk_cpu_freq_get_config(&config);
    // rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_2M, &config);
    // rtc_clk_cpu_freq_set_config_fast(&config);


    delay(1000);

    Serial.println("HI");
    NimBLEDevice::init("NimBLE Scanner");
    AcaiaScalesPlugin::apply();
    // RemoteScalesPluginRegistry::getInstance()->
    scanner.initializeAsyncScan();

}


void logCallback(std::string string) {
    Serial.println(string.c_str());
}

void weightCallback(float newWeight) {
    Serial.print("Received weight: ");
    Serial.println(newWeight, 4);
}

void loop()
{
    if (state == PROGRAM_STATE::SCAN_FOR_SCALES) {
        if (!scanner.getDiscoveredScales().size()) {
            Serial.println("STILL SCANNING");
        }
        else {
            Serial.println("OMG");
            scanner.stopAsyncScan();
            auto scales = scanner.getDiscoveredScales();
            remote_scale = std::move(RemoteScalesPluginRegistry::getInstance()->initialiseRemoteScales(scales[0]));
            remote_scale->setLogCallback(logCallback);
            remote_scale->setWeightUpdatedCallback(weightCallback);

            state = PROGRAM_STATE::SCALE_FOUND;
        }
    }
    else if (state == PROGRAM_STATE::SCALE_FOUND) {
        if(remote_scale) {
            Serial.println("YEHEY");
            remote_scale->connect();
            if(remote_scale->isConnected()) {
                Serial.println("OMG2");
                Serial.println("DISCOVERED DEVICE");
                Serial.print("name:");
                Serial.println(remote_scale->getDeviceName().c_str());
                Serial.print("address:");
                Serial.println(remote_scale->getDeviceAddress().c_str());
                state = PROGRAM_STATE::SCALE_CONNECTED;
            }
            else {
                Serial.println("make it work..");
            }
        }

    }
    else if (state == PROGRAM_STATE::SCALE_CONNECTED) {
        Serial.println("waiting..");
        if(!remote_scale->isConnected()) {
            Serial.println("Disconnected!");
            state = PROGRAM_STATE::SCALE_FOUND;
            remote_scale->disconnect();
        }
    }
    delay(1000);
    // RemoteScalesPluginRegistry::getInstance()->
}