#ifndef SONG_RUNNER_H
#define SONG_RUNNER_H
#include <Arduino.h>
#include "pin.h"

// paramétres
const long note_time = 500;

#define max_boucle 10



const long track[][3]={ // do:261  ré:293  mi:330  fa:350  sol:391  la:440  si:493
  { 1*note_time,261,note_time-100},
  { 2*note_time,261,note_time-100},
  { 3*note_time,261,note_time-100},
  { 4*note_time,293,note_time-100},
  { 5*note_time,330,2*note_time-100},
  { 7*note_time,293,note_time-100},
  { 8*note_time,261,note_time-100},
  { 9*note_time,330,note_time-100},
  { 10*note_time,293,note_time-100},
  {11*note_time,293,note_time-100},
  {12*note_time,261,note_time-100},
  {13*note_time,0,0},
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
