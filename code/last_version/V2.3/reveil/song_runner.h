#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "pin.h"
#include "software_parameters.h"


const long track[][3]={ // do:261  ré:293  mi:330  fa:350  sol:391  la:440  si:493
  { 1*NOTE_TIME,261,NOTE_TIME-100},
  { 2*NOTE_TIME,261,NOTE_TIME-100},
  { 3*NOTE_TIME,261,NOTE_TIME-100},
  { 4*NOTE_TIME,293,NOTE_TIME-100},
  { 5*NOTE_TIME,330,2*NOTE_TIME-100},
  { 7*NOTE_TIME,293,NOTE_TIME-100},
  { 8*NOTE_TIME,261,NOTE_TIME-100},
  { 9*NOTE_TIME,330,NOTE_TIME-100},
  { 10*NOTE_TIME,293,NOTE_TIME-100},
  {11*NOTE_TIME,293,NOTE_TIME-100},
  {12*NOTE_TIME,261,NOTE_TIME-100},
  {13*NOTE_TIME,0,0},
  {-1,0,0}
};


class song_runner{
  public:
  song_runner();
  void start();
  int run();
  void stop();
  bool is_active();
  private:
  int boucle;
  long t0=0;
  int next_step = 0;
  bool active = false;
};



#endif
