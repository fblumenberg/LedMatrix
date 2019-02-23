
#ifndef Mqtt_h
#define Mqtt_h

#include <map>

#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>
#include "Config.h"

class Mqtt
{
  private:
    PubSubClient *_client;
    Config *_config;

    std::map<String, std::function<void(char *, uint8_t *, unsigned int)>> _cbMap;

    std::function<void()> _connectCallback;

    void reconnect();

  public:
    Mqtt(PubSubClient &client, Config &config);

    Mqtt &setTopicCallback(const char *topic, MQTT_CALLBACK_SIGNATURE);

    Mqtt &setConectCallback(std::function<void()> callback)
    {
        _connectCallback = callback;
        return *this;
    }

    void loop();

    void callback(char *topic, byte *payload, unsigned int length);
};

#endif