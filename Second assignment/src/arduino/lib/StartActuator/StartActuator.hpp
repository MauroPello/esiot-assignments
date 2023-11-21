#ifndef START_ACTUATOR_HPP
#define START_ACTUATOR_HPP

class StartActuator { 
public:
    StartActuator(int pin);
    bool isActive();
private:
    int pin;  
};

#endif