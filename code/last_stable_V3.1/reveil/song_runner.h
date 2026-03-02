#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "software_parameters.h"
#include "pin.h"

// Déclaration pour que le .ino et le .cpp voient le même objet
extern DFRobotDFPlayerMini myDFPlayer; 

void song_setup();
void printDetail(uint8_t type, int value);
#define SONG_SETUP_NEEDED

class song_runner {
  public:
    song_runner();
    void start();
    void stop();
    int run();
    int Alert(int ID);
    int End_Alert();
    bool is_active();
  private:
    unsigned long t0 = 0;
};
#endif
