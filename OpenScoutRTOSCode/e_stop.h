#ifndef E_STOP_H
#define E_STOP_H

// === E-STOP PIN ===
extern const int ESTOP_PIN;

// === E-STOP STATE ===
extern volatile bool eStopPressed;
extern volatile bool eStopLatched;

// === FUNCTION DECLARATIONS ===
void initializeEStop();
bool isEStopActive();
void resetEStop();
bool isEStopButtonPressed();
void eStopISR();

#endif // E_STOP_H
