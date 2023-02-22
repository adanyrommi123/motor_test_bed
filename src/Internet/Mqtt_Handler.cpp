#include "Mqtt_Handler.h"

// LED Pin
const int ledPin = 26;

Mqtt_Handler mqttHandler;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char *ntpServer = "pool.ntp.org";

void Mqtt_Handler::setup_mqtt()
{   
    pinMode(ledPin, OUTPUT);
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);
    // init configTime
    configTime(0, 0, ntpServer);
}

void Mqtt_Handler::reconnect_loop()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Subscribe
      mqttClient.subscribe("ketimpangan/switch");
      mqttClient.subscribe("ketimpangan/LED");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void Mqtt_Handler::loop() 
{
    if (!mqttClient.connected())
    {
        reconnect_loop();
    }
    mqttClient.loop();
}

void Mqtt_Handler::jsonFormatSensorValue(SensorsHandler_Value& value)
{   
    // get UNIX time
    unsigned long epochTime = getTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);

    String clientId = WiFi.macAddress();
    clientId.replace(':',' ');
    clientId.toUpperCase();

                    //      ",\"BV\":" + String(value.bus_voltage) + 
                    //  ",\"CUR\":" + String(value.current) + 
                    //  ",\"LV\":" + String(value.load_voltage) + 
                    //  ",\"P\":" + String(value.power) + 
                    //  ",\"RPM\":" + String(value.rpm_data) + 
                    //  ",\"SV\":" + String(value.shunt_voltage) +
    
    dataFormatjson = "{\"data\":{\"AMB\":" + String(value.ambient_temp) +
                     ",\"OBJ\":" + String(value.object_temp) + 
                     ",\"BV\":" + String(value.bus_voltage) + 
                     ",\"CUR\":" + String(value.current) + 
                     ",\"LV\":" + String(value.load_voltage) + 
                     ",\"P\":" + String(value.power) + 
                     ",\"RPM\":" + String(value.rpm_data) + 
                     ",\"SV\":" + String(value.shunt_voltage) + "}";

    dataFormatjson += ",\"time\":" + String(epochTime) + ",\"clientId\":\"" + String(clientId) + "\"}";

    Serial.println("Sending the data : ");
    Serial.print("Packet : ");
    Serial.println(dataFormatjson);
}

void Mqtt_Handler::mqttPublishSensorValue(String _topic)
{
    mqttClient.publish(_topic.c_str(), dataFormatjson.c_str(), true);
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "motortestbed/parameters")
  {
    Serial.print("Changing output to ");
    if (messageTemp == "on")
    {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off")
    {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
  if (String(topic) == "ketimpangan/switch")
  {
    Serial.print("Changing output to ");
    if (messageTemp == "on")
    {
      Serial.println("on");
      sensor_switch = true;
    }
    else if (messageTemp == "off")
    {
      sensor_switch = false;
    }
  }
}

//Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println ("Failed to obtain time");
  }
  time (&now);
  return now;
}
