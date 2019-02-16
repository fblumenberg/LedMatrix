#pragma once

typedef struct
{
  bool doConnect;

  String mqttClientName;
  String mqttServer;
  String mqttTopic;
  int mqttPort = 1883;

} Config;

extern Config config;

extern bool loadConfig();
extern bool saveConfig();
