#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "pin.h"
#include "software_parameters.h"
#include <DFRobotDFPlayerMini.h>


class song_runner{
  public:
  song_runner();
  void start();
  int run();
  void stop();
  bool is_active();
  private:
  bool active = false;
};



#endif
