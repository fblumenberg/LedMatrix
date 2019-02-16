#include <ArduinoJson.h>
#include "FS.h"
//#include "SPIFFS.h"
#include "Config.h"

Config config;

bool loadConfig()
{
  // start with a sane default
  config.mqttClientName = "ESP" + String(ESP.getChipId());

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024)
  {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(buf.get());

  if (!json.success())
  {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char *jsonString;

  config.doConnect = json["doConnect"];

  jsonString = json["mqttServer"];
  config.mqttServer = String((const char *)json["mqttServer"]);

  jsonString = json["mqttClientName"];
  config.mqttClientName = jsonString; //String(json["mqttClientName"]);

  jsonString = json["mqttTopic"];
  config.mqttTopic = jsonString; //String(json["mqttClientName"]);

  config.mqttPort = json["mqttPort"];

  // Real world application would store these values in some variables for
  // later use.
  Serial.print("Loaded serverName: ");
  Serial.println(config.mqttServer);
  return true;
}

bool saveConfig()
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  json["mqttServer"] = config.mqttServer;
  json["mqttPort"] = config.mqttPort;
  json["mqttClientName"] = config.mqttClientName;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
