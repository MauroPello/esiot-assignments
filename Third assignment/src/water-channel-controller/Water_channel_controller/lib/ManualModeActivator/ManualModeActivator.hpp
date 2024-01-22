#ifndef MANUAL_MODE_ACTIVATOR_HPP
#define MANUAL_MODE_ACTIVATOR_HPP

class ManualModeActivator {
  public:
    ManualModeActivator(int pin);
    bool isActive();

  private:
    int pin;
};

#endif