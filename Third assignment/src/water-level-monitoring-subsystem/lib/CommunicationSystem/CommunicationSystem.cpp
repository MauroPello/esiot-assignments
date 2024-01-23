#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <CommunicationSystem.hpp>

const char *ssid = "HOME";
const char *password = "zuppadavid123";
const char *mqtt_server = "broker.mqtt-dashboard.com"; // TODO capire sta cosa
const char *sendTopic = "water-level-topic";
const char *receiveTopic = "system-state";
WiFiClient espClient;
PubSubClient client(espClient);
char msgIn[MSG_BUFFER_SIZE];
char msgOut[MSG_BUFFER_SIZE];

void callbackReceive(char *topic, byte *payload, unsigned int length)
{
   for (unsigned int i = 0; i < length; i++) {
       msgIn[i] = (char)payload[i];
   }
   msgIn[length] = '\0';
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
    client.setCallback(callbackReceive);
    client.subscribe(receiveTopic);
};

bool CommunicationSystem::isConnected()
{
    return client.connected();
}

void CommunicationSystem::reconnect()
{
    String clientId = String("water-level-monitoring") + String(random(0xffff), HEX);

    while (!client.connect(clientId.c_str()))
    {
        delay(1000);
    }
    client.subscribe(receiveTopic);
}

void CommunicationSystem::sendData(String data)
{
    snprintf(msgOut, MSG_BUFFER_SIZE, "%s", data);
    Serial.println("Mandando il messaggio:" + String(msgOut));
    client.publish(sendTopic, msgOut);
}

String CommunicationSystem::receiveData()
{
    client.loop();
    String receivedMessage = String(msgIn);
    msgIn[0] = '\0';
    return receivedMessage;
}
