#include "song_runner.h"

void song_setup(){
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/BUZZER_SPEAKER_RX, /*tx =*/BUZZER_SPEAKER_TX);
  delay(1000);
  myDFPlayer.volume(SONG_VOLUME);  //Set volume value. From 0 to 30
}

song_runner::song_runner(){
  //
}

void song_runner::start(){
  myDFPlayer.loop(1);  //Loop the first mp3
  t0 = millis();
}


int song_runner::run(){
  if(MAX_PLAY_TIME>(millis()-t0) && is_active()){
    stop();
  }
}


void song_runner::stop(){
  myDFPlayer.sleep();  //pause the mp3
}

bool song_runner::is_active(){
  return myDFPlayer.readState()==1;
}
