#include <Arduino.h>

// Library-library OOP
#include "Internet/Wifi_Handler.h"
#include "Internet/Mqtt_Handler.h"
#include "Sensors/Sensors_Handler.h"

#define POTENSIO_PIN  36 // Pin Potentiometer A0
#define RPWM_PIN      18 // Digital Pin 5 Arduino Arduino

bool sensor_switch = true;

long lastMsg = 0;
char msg[50];
int value = 0;

void setup()
{
  pinMode(RPWM_PIN, OUTPUT);
  digitalWrite(RPWM_PIN, LOW);
  Serial.begin(115200);
  delay(2000);
  wifiHandler.wifiSetup();
  delay(2000);
  mqttHandler.setup_mqtt();
  delay(1000);
  sensorsHandler.begin();
}

void loop()
{ 
  if (!wifiHandler.isConnected()) {
      // Try to reconnecting if wifi not connected
      delay(500);
      wifiHandler.reconnect();
  }

  int potentioValue = analogRead(POTENSIO_PIN);

  if (potentioValue<=0){
  potentioValue = 0;
  }
  else if(potentioValue>=4095){
  potentioValue = 4096;
  }
 
  int outputDAC = map(potentioValue, 0, 4096, 0, 255);

  analogWrite(RPWM_PIN, outputDAC);

  mqttHandler.loop();

  long now = millis();
  if (now - lastMsg > 2000)
  { 
    lastMsg = now;
    // Temperature in Celsius
    // if (sensor_switch == true)
    // {   
        // Local struct object
        SensorsHandler_Value sensorsValue;
        sensorsValue.ambient_temp = -9999.0;
        sensorsValue.object_temp  = -9999.0;
        sensorsValue.bus_voltage  = -9999.0;
        sensorsValue.current      = -9999.0;
        sensorsValue.load_voltage = -9999.0;
        sensorsValue.object_temp  = -9999.0;
        sensorsValue.power        = -9999.0;
        sensorsValue.rpm_data     = -9999.0;
        sensorsValue.shunt_voltage= -9999.0;
        

        sensorsHandler.getValue(sensorsValue);
        Serial.print("Potentiometer:");Serial.println(potentioValue);
        mqttHandler.jsonFormatSensorValue(sensorsValue);
        
        mqttHandler.mqttPublishSensorValue("motortestbed/parameters");
    // }
  }
}
