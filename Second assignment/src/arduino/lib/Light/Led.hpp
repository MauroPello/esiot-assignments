#ifndef LED_HPP
#define LED_HPP
#include <Light.h>

class Led: public Light { 
public:
    Led(int pin);
    void switchOn();
    void switchOff();    
private:
    int pin;  
};

#endif