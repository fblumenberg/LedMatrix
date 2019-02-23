#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <ArduinoOTA.h>

#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>

#include <ArduinoJson.h>

#include "Config.h"
#include "Ota.h"
#include "Mqtt.h"
#include "LedMatrix.h"

#include "Patterns.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

#define TRIGGER_PIN 0

WiFiClient espClient;
PubSubClient client(espClient);

Mqtt mqtt(client, config);

Patterns patterns;

static bool shouldSaveConfig = false;

void tick()
{
    //toggle state
    int state = digitalRead(LED_BUILTIN); // get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);    // set pin to the opposite state
    matrix.ToggleStatus();
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
    matrix.ShowWifi();
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

    // wifiManager.resetSettings();
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
    digitalWrite(LED_BUILTIN, HIGH);
    matrix.StatusOff();
}

void mqttPattern(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("New pattern %s - %d\n", topic, length);

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject &root = jsonBuffer.parseObject(payload);

    Serial.printf("Parsed, %d\n", root.containsKey("pattern"));

    if (root.containsKey("pattern") && root["pattern"].is<const char *>())
    {
        const char *pattern = root["pattern"];
        Serial.printf("Set pattern %s\n", pattern);
        patterns.SetPattern(pattern);
    }
    else
    {
        Serial.println("The given pattern is not a string");
    }
}

void publishInfo()
{
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject &JSONencoder = JSONbuffer.createObject();

    JsonArray &values = JSONencoder.createNestedArray("patterns");

    patterns.GetList(values);

    char JSONmessageBuffer[300];
    int lenghtPretty = JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    Serial.println(lenghtPretty);
    Serial.println("\nPretty JSON message from buffer: ");

    const char* patternTopic = "Ledmatrix/info";
    bool published= client.publish(patternTopic, JSONmessageBuffer, true);

    Serial.printf("Published under %s - %d\n",patternTopic,published);
}

void mqttGetInfo(char *topic, byte *payload, unsigned int length){
    publishInfo();
}

void mqttConnected()
{
    Serial.println("MQTT connected");

    publishInfo();

    mqtt.setTopicCallback("Ledmatrix/pattern", mqttPattern);
    mqtt.setTopicCallback("Ledmatrix/getInfo", mqttGetInfo);
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);

    Serial.println("\n Starting");
    Serial.println("LED Matrix!");

    //read configuration from FS json
    Serial.println("mounting FS...");

    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }

    matrix.Setup();

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

    setupOTA();
    delay(10);

    pinMode(TRIGGER_PIN, INPUT);

    mqtt.setConectCallback(mqttConnected);

    patterns.Start();

}

unsigned long timer = millis();
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
    if (timer < millis())
    {
        unsigned int d = patterns.DrawFrame();
        matrix.Show();
        timer += d;
    }

    delay(1);
}
