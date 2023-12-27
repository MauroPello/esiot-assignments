#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <CommunicationSystem.hpp>

const char *ssid = "speziali";
const char *password = "L1ks1pDX";
const char *mqtt_server = "broker.mqtt-dashboard.com";
const char *topic = "water-level-monitor";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[MSG_BUFFER_SIZE];

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.println(String("Message arrived on [") + topic + "] len: " + length);
}

void setup_wifi()
{
    delay(10);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
}

CommunicationSystem::CommunicationSystem()
{
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
};

bool CommunicationSystem::isConnected()
{
    return client.connected();
}

void CommunicationSystem::reconnect()
{
    String clientId = String("esiot-2122-client-") + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str()))
    {
        // Once connected, publish an announcement...
        // client.publish("outTopic", "hello world");
        // ... and resubscribe
        client.subscribe(topic);
    }
    // TODO metti in main di aspettare tot secondi prima di riprovare
}

void CommunicationSystem::sendData(char *data)
{
	snprintf(msg, MSG_BUFFER_SIZE, data);
    client.publish(topic, msg);
}

char* CommunicationSystem::receiveData()
{
	client.loop();
}
