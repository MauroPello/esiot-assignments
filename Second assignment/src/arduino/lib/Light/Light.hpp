#ifndef LIGHT_HPP
#define LIGHT_HPP

class Light {
  public:
    virtual void switchOn() = 0;
    virtual void switchOff() = 0;
};

#endif