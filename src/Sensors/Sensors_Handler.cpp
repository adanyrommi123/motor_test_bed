#include "Sensors_Handler.h"

Sensors_Handler sensorsHandler;

MLX90615 mlx = MLX90615();
Adafruit_INA219 ina219;
const byte pin_rpm = 34;
int volatile rev = 0;
int rpm = 0;
unsigned long cur_time, old_time;

void isr(){
   rev++;
}

void Sensors_Handler::begin()
{
    mlx.begin();
    Serial.println("Melexis MLX90615 infra-red temperature sensor test");
    Serial.print("Sensor ID number = ");
    Serial.println(mlx.get_id(), HEX);

    // function begin dari sensor arus
    if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    // while (1) { delay(10);}


  }
   pinMode(pin_rpm,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_rpm), isr, FALLING);
}

void Sensors_Handler::getValue(SensorsHandler_Value& sensorsValue)
{
      sensorsValue.ambient_temp = mlx.get_ambient_temp();
      sensorsValue.object_temp = mlx.get_object_temp();
      // Uncomment the next line to set temperature in Fahrenheit
      // (and comment the previous temperature line)
      // temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
      // Convert the value to a char array
      sensorsValue.shunt_voltage = ina219.getShuntVoltage_mV();
      sensorsValue.bus_voltage = ina219.getBusVoltage_V();
      sensorsValue.current = ina219.getCurrent_mA();
      sensorsValue.power = ina219.getPower_mW();
      sensorsValue.load_voltage = sensorsValue.bus_voltage + (sensorsValue.shunt_voltage / 1000);

      cur_time = millis();
      if(cur_time-old_time>=1000) {
         detachInterrupt(digitalPinToInterrupt(pin_rpm));
         sensorsValue.rpm_data=rev*60/4;
         Serial.print("REV: ");Serial.println(rev);
         Serial.print("RPM: ");Serial.println(sensorsValue.rpm_data);
         rev=0;
         attachInterrupt(digitalPinToInterrupt(pin_rpm), isr, FALLING);
         old_time=millis();
      }

      char ambient_tempString[8];
      char object_tempString[8];
      char shunt_voltage_tempString[8];
      char bus_voltage_tempString[8];
      char current_tempString[8];
      char power_tempString[8];
      char load_voltage_tempString[8];

      dtostrf(sensorsValue.ambient_temp, 1, 2, ambient_tempString);
      dtostrf(sensorsValue.object_temp, 1, 2, object_tempString);
      dtostrf(sensorsValue.shunt_voltage, 1, 2, shunt_voltage_tempString);
      dtostrf(sensorsValue.bus_voltage, 1, 2, bus_voltage_tempString);
      dtostrf(sensorsValue.current, 1, 2, current_tempString);
      dtostrf(sensorsValue.power, 1, 2, power_tempString);
      dtostrf(sensorsValue.load_voltage, 1, 2, load_voltage_tempString);
      
      // Wattmeter Data Sensor (INA219)
      Serial.print("Ambient = ");
      Serial.print(sensorsValue.ambient_temp);
      Serial.print(" *C\tObject = ");
      Serial.print(sensorsValue.object_temp);
      Serial.println(" *C");
      // Wattmeter Data Sensor (INA219)
      Serial.print("Bus Voltage:   "); Serial.print(sensorsValue.bus_voltage); Serial.println(" V");
      Serial.print("Shunt Voltage: "); Serial.print(sensorsValue.shunt_voltage); Serial.println(" mV");
      Serial.print("Load Voltage:  "); Serial.print(sensorsValue.load_voltage); Serial.println(" V");
      Serial.print("Current:       "); Serial.print(sensorsValue.current); Serial.println(" mA");
      Serial.print("Power:         "); Serial.print(sensorsValue.power); Serial.println(" mW");

}