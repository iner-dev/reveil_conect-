#include "song_runner.h"

song_runner::song_runner(){
  //
}

void song_runner::start(){
  Serial.println("Ringing");
  t0 = millis();
  next_step = 0;
  boucle = 1;
  active = true;
}


int song_runner::run(){
  if((millis()-t0>track[next_step][0]||track[next_step][0]<0 )&& active){
    if(track[next_step][0]<0){ // cas spéciaux
      if  (track[next_step][0]==-2||boucle==MAX_BOUCLE)   stop();
      else if       (track[next_step][0]==-1){
        t0 = millis();
        next_step = 0;
        boucle++;
      }
      return 2;
    }else{
      if(track[next_step][1]>0){  // cas d'execution de note
        noTone(BUZZER_PIN);
        tone(BUZZER_PIN,track[next_step][1],track[next_step][2]);
      }else{ // cas d'arret buzzer
        noTone(BUZZER_PIN);
      }
      next_step++;
      return 1;
    }
  }
  return 0;
}


void song_runner::stop(){
  active = false;
  noTone(BUZZER_PIN);
  Serial.println("Stop ringing");
}

bool song_runner::is_active(){
  return active;
}
