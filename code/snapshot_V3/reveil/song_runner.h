#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

// Déclaration pour que le .ino et le .cpp voient le même objet
extern DFRobotDFPlayerMini myDFPlayer; 

void song_setup();
#define SONG_SETUP_NEEDED

class song_runner {
  public:
    song_runner();
    void start();
    void stop();
    int run();
    bool is_active();
  private:
    unsigned long t0 = 0;
};
#endif
