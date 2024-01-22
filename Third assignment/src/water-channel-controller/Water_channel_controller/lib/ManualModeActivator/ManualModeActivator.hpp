#ifndef MANUAL_MODE_ACTIVATOR_HPP
#define MANUAL_MODE_ACTIVATOR_HPP

class ManualModeActivator {
  public:
    ManualModeActivator(int pin, void (*handler)(void));
    bool isActive();

  private:
    int pin;
    void (*handler)(void);
};

#endif