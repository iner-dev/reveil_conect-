#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "pin.h"
#include "software_parameters.h"
#include "DFRobotDFPlayerMini.h"

#define FPSerial Serial2

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
void song_setup();
#define SONG_SETUP_NEEDED

class song_runner{
  public:
  song_runner();
  void start();
  int run();
  void stop();
  bool is_active();
  private:
  unsigned long t0 = 0;
};



#endif
