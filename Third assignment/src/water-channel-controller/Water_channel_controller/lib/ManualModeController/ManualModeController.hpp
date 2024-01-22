#ifndef MANUAL_MODE_CONTROLLER_HPP
#define MANUAL_MODE_CONTROLLER_HPP

class ManualModeController {
  public:
    ManualModeController(int pin);
    int getValveLevel();

  private:
    int pin;
};

#endif