#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <ArduinoOTA.h>

#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <PubSubClient.h>

#include "Config.h"
#include "Ota.h"
#include "Mqtt.h"
//for LED status
#include <Ticker.h>
Ticker ticker;

#define TRIGGER_PIN 0

WiFiClient espClient;
PubSubClient client(espClient);

Mqtt mqtt(client, config);

// static void connectToWiFi();
static void mqttReconnect();

static bool shouldSaveConfig = false;

void tick()
{
    //toggle state
    int state = digitalRead(LED_BUILTIN); // get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);    // set pin to the opposite state
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

//callback notifying us of the need to save config
void saveConfigCallback()
{
    Serial.println("Should save config");
    shouldSaveConfig = true;
}

void connectToWifi(WiFiManager &wifiManager, bool autoConnect)
{
    //set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);
    // start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);

    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    WiFiManagerParameter custom_mqtt_name("name", "mqtt name", String(config.mqttClientName).c_str(), 20);
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", config.mqttServer.c_str(), 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", String(config.mqttPort).c_str(), 6);

    //wifiManager.resetSettings();
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    //add all your parameters here
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_name);

    shouldSaveConfig = false;

    if (autoConnect)
    {
        wifiManager.autoConnect();
    }
    else
    {
        wifiManager.startConfigPortal();
    }

    //read updated parameters
    config.mqttServer = custom_mqtt_server.getValue();
    config.mqttClientName = custom_mqtt_name.getValue();
    config.mqttPort = atoi(custom_mqtt_port.getValue());

    //save the custom parameters to FS
    if (shouldSaveConfig)
    {
        saveConfig();
    }

    ticker.detach();
    //keep LED on
    digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);

    Serial.println("\n Starting");
    Serial.println("OTA Base!");

    //read configuration from FS json
    Serial.println("mounting FS...");

    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }

    if (!loadConfig())
    {
        Serial.println("Failed to load config");
    }
    else
    {
        Serial.println("Config loaded");
    }

    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);

    connectToWifi(wifiManager, true);

    // connectToWiFi();
    setupOTA();
    delay(10);

    pinMode(TRIGGER_PIN, INPUT);
}

void loop()
{
    loopOTA();

    // is configuration portal requested?
    if (digitalRead(TRIGGER_PIN) == LOW)
    {
        //WiFiManager
        //Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;
        wifiManager.setConfigPortalTimeout(120);
        connectToWifi(wifiManager, false);
    }

    mqtt.loop();
    delay(100);
}
