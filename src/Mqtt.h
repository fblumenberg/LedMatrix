
#ifndef Mqtt_h
#define Mqtt_h

#include <map>

#include <PubSubClient.h>
#include "Config.h"

class Mqtt{
private:

   PubSubClient* _client;
   Config* _config;

   std::map<String, std::function<void(char*, uint8_t*, unsigned int)>> _cbMap;

    void reconnect();
    
public:
    Mqtt(PubSubClient& client,Config& config);

    Mqtt& setTopicCallback(const char* topic, MQTT_CALLBACK_SIGNATURE);

    void loop();

    void callback(char *topic, byte *payload, unsigned int length);
};


#endif