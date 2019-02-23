#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include "Mqtt.h"

static Mqtt *mqtt;
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    mqtt->callback(topic, payload, length);
}

Mqtt::Mqtt(PubSubClient &client, Config &config)
{
    mqtt = this;
    this->_client = &client;
    this->_config = &config;
    this->_connectCallback = NULL;
    _client->setCallback(mqttCallback);
}

Mqtt &Mqtt::setTopicCallback(const char *topic, MQTT_CALLBACK_SIGNATURE)
{
    _cbMap.insert(std::make_pair(topic, callback));
    _client->subscribe(topic);
    return *this;
}

void Mqtt::loop()
{
    if (!_client->connected())
    {
        reconnect();
    }

    _client->loop();
}

void Mqtt::reconnect()
{
    // Loop until we're reconnected
    while (!_client->connected())
    {
        _client->setServer(config.mqttServer.c_str(), config.mqttPort);

        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (_client->connect(
                config.mqttClientName.c_str(),
                "died",
                MQTTQOS1, true,
                config.mqttClientName.c_str()))
        {
            Serial.print("connected ");
            Serial.println(_client->connected());

            // Once connected, publish an announcement...
            _client->publish("clients", _config->mqttClientName.c_str());

            if (_connectCallback)
                _connectCallback();
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(_client->state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        yield();
    }

    Serial.print("reconnected ");
    Serial.print("mqtt state ");
    Serial.print(_client->state());
    Serial.print(" connected ");
    Serial.println(_client->connected());
}

void Mqtt::callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("MqttCB ");
    Serial.println(topic);
    if (_cbMap.find(topic) != _cbMap.end())
    {
        _cbMap[topic](topic, payload, length);
    }
}
