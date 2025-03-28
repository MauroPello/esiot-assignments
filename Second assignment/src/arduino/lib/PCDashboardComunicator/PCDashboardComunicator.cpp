#include <PCDashboardComunicator.hpp>

#define TEMP_PREFIX "T:"
#define STATE_PREFIX "S:"
#define N_WASHES_PREFIX "N:"
#define SUFFIX ";"

PCDashboardComunicator::PCDashboardComunicator() {}

String PCDashboardComunicator::readSerialString() {
    String inputString = "";
    while (Serial.available() > 0) {
        char incomingChar = Serial.read();
        if (incomingChar == '\n') {
            break;
        }
        inputString += incomingChar;
        delay(2);
    }
    return inputString;
}

void PCDashboardComunicator::sendMessage(String prefix, String value) {
    while (Serial.availableForWrite()==0);
    Serial.print(prefix + value + SUFFIX);
}

void PCDashboardComunicator::sendNumberOfWashes(int n) {
    sendMessage(N_WASHES_PREFIX, String(n));
}

void PCDashboardComunicator::sendState(String state) {
    sendMessage(STATE_PREFIX, state);
}

void PCDashboardComunicator::sendTemperature(float temp) {
    sendMessage(TEMP_PREFIX, String(temp));
}

bool PCDashboardComunicator::isMaintenanceDone() {
    return readSerialString().equals("SOLVED");
}
