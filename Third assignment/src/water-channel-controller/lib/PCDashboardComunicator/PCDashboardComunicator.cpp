#include <PCDashboardComunicator.hpp>

#define VALVE_PREFIX "V: "
#define STATE_PREFIX "S: "
#define DELIMITER " | "
#define SUFFIX ";"

PCDashboardComunicator::PCDashboardComunicator() {}


void PCDashboardComunicator::sendMessage(String prefix, String value) {
    while (Serial.availableForWrite() == 0);
    Serial.print(prefix + value + SUFFIX);
}

void PCDashboardComunicator::sendStateAndLevel(String state, int percentage) {
    while (Serial.availableForWrite() == 0);
    Serial.print(VALVE_PREFIX + String(percentage) + DELIMITER + STATE_PREFIX + state + SUFFIX);
}

int PCDashboardComunicator::getValveLevel() {
    String inputString = "";
    bool getValue = false;
    while (Serial.available() > 0) {
        analogWrite(11, 255);
        delay(50);
        analogWrite(11, 0);
        char incomingChar = Serial.read();
        if (incomingChar == ';') {
            break;
        }
        if (incomingChar == ':') {
            getValue = true;
        }
        if (getValue && incomingChar != ' ' && incomingChar != ':') {
            inputString += incomingChar;
        }
        delay(2);
    }
    return inputString.length() == 0 ? -1 : inputString.toInt();
}
