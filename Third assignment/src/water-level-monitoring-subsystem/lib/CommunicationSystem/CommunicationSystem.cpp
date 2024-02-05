#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <CommunicationSystem.hpp>

const char *ssid = "HOME";
const char *password = "zuppadavid123";
const char *mqtt_server = "192.168.1.130";
const char *sendTopic = "water-level-topic";
const char *receiveTopic = "system-state";
IPAddress ip(192, 168, 1, 69);
IPAddress router(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
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
    WiFi.config(ip, router, subnet, router);
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
