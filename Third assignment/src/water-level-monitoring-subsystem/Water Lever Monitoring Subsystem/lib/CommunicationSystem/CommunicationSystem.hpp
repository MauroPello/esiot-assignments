#ifndef COMMUNICATION_SYSTEM_HPP
#define COMMUNICATION_SYSTEM_HPP

#define MSG_BUFFER_SIZE 50


class CommunicationSystem {
  private:

  public:
    CommunicationSystem();
    bool isConnected();
    void reconnect();
    void sendData(char* data);
    char *receiveData();
};

#endif