#include "song_runner.h"

DFRobotDFPlayerMini myDFPlayer;

void song_setup() {
  FPSerial.begin(9600, SERIAL_8N1, BUZZER_SPEAKER_RX, BUZZER_SPEAKER_TX);
  
  // Laisser le temps au module de s'allumer
  delay(2000); 

  Serial.println(F("Initialisation du DFPlayer..."));
  if (!myDFPlayer.begin(FPSerial)) {
    Serial.println(F("Erreur : DFPlayer non détecté ! Vérifiez le câblage."));
    // On ne continue pas si ça échoue pour éviter le crash
    return; 
  }
  
  myDFPlayer.volume(SONG_VOLUME);
  Serial.println(F("DFPlayer prêt !"));
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
