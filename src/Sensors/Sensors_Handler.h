#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <mlx90615.h>
#include <Adafruit_INA219.h>

typedef struct
{
    float ambient_temp;
    float object_temp;
    float current;
    float shunt_voltage;
    float bus_voltage;
    float load_voltage;
    float power;
    int rpm_data;
} SensorsHandler_Value;

class Sensors_Handler
{
    public:
        void begin();
        void getValue(SensorsHandler_Value& sensorsValue);
        
};

void isr();

extern Sensors_Handler sensorsHandler;