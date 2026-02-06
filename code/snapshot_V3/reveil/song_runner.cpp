#include "song_runner.h"

song_runner::song_runner(){
  //
}

void song_runner::start(){
  active = true;
}


int song_runner::run(){
}


void song_runner::stop(){
  active = false;
}

bool song_runner::is_active(){
  return active;
}
