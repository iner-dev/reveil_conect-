#include "calendar.h"
#include "song_runner.h"
#include "pin.h"


// before compile you have to modify icalUrl and in calendar.cpp : ssid and password
char* icalUrl = "your Ical URL"; 

calendar gcal(icalUrl);
song_runner music;
time_t next_alarm = 32503680000;
time_t last_update;
time_t now;

bool updated = false;
bool secret_mode = false;

void reset(){
  WiFi.disconnect();
  ESP.restart();
  while(true){};
}

void update(){
  digitalWrite(ERROR_LED_PIN,HIGH);
  updated = gcal.update();
  if (updated){
    next_alarm = gcal.next_event_named("reveil");
    now = time(nullptr);
    Serial.println("next event at "+String(next_alarm)+"UTC in "+String(next_alarm-now)+"sec");
  }
  last_update = time(nullptr);
  digitalWrite(ERROR_LED_PIN,LOW);
}

void debug(){
  bool debug_mode = true;
  char serial;
  Serial.println("Debug mode Enable");
  while(debug_mode){
    if(Serial.available()){
      serial = Serial.read();
      switch(serial){
        case 'I' :
          gcal.debug_inject();
          break; 
        case 'E' :
          debug_mode = false;
          break; 
        case 'B' :
          tone(BUZZER_PIN,440);
          Serial.println("buzzer ON");
          break; 
        case 'b' :
          noTone(BUZZER_PIN);
          Serial.println("buzzer OFF");
          break; 
        case 'L' :
          digitalWrite(ERROR_LED_PIN,HIGH);
          Serial.println("LED ON");
          break; 
        case 'l' :
          digitalWrite(ERROR_LED_PIN,LOW);
          Serial.println("LED OFF");
          break; 
        case 'R' :
          reset();
          break; 
        case 'U' :
          update();
          break; 
        case 'N' :
          Serial.println("next event at "+String(next_alarm)+"UTC in "+String(next_alarm-now)+"sec");
          break; 
      }
    }
    if(digitalRead(STOP_ALARM_PIN)) Serial.println("STOP_ALARM_PIN pressed");
    if(!digitalRead(TEST_ALARM_PIN)) Serial.println("TEST_ALARM_PIN pressed");
    if(!digitalRead(UPDATE_PIN)) Serial.println("UPDATE_PIN pressed");
    if(!digitalRead(SOFT_RESET_PIN)) Serial.println("SOFT_RESET_PIN pressed");
  }
  Serial.println("Debug mode Disable");
}


void setup() {
  Serial.begin(115200);
  pinMode(STOP_ALARM_PIN,INPUT_PULLUP);
  pinMode(TEST_ALARM_PIN,INPUT_PULLUP);
  pinMode(UPDATE_PIN,INPUT_PULLUP);
  pinMode(SOFT_RESET_PIN,INPUT_PULLUP);
  pinMode(ERROR_LED_PIN,OUTPUT);
  digitalWrite(ERROR_LED_PIN,LOW);
  delay(1000); 
  noTone(BUZZER_PIN);
  
  while(!digitalRead(SOFT_RESET_PIN)) {
    delay(100);
    if(!digitalRead(TEST_ALARM_PIN) && !digitalRead(UPDATE_PIN)) debug();
  }
  digitalWrite(ERROR_LED_PIN,HIGH);
  
  update();

  Serial.println("Booted");
}



void loop() {
  //*
  now = time(nullptr);
  
  if(secret_mode){
    digitalWrite(ERROR_LED_PIN,(int(millis()/500)%2==0));
    if(digitalRead(STOP_ALARM_PIN) && digitalRead(TEST_ALARM_PIN)){
      tone(BUZZER_PIN,440);
    }else noTone(BUZZER_PIN);
  }else if(!digitalRead(TEST_ALARM_PIN) && digitalRead(STOP_ALARM_PIN)){
    Serial.println("secret enable");
    secret_mode = true;
    music.stop();
  }else if(!updated){
    digitalWrite(ERROR_LED_PIN,(int(millis()/200)%2==0));
  }
  if(next_alarm<now){
    Serial.println("waking up buddy");
    music.start();
    if (updated) next_alarm = gcal.next_event_named("reveil");
  }
  if(secret_mode && !digitalRead(UPDATE_PIN)){ 
    secret_mode = false;
    while(!digitalRead(UPDATE_PIN))delay(100);
    digitalWrite(ERROR_LED_PIN,LOW);
  }
  if(( ( (last_update+3600)<now && ((next_alarm-300)>now)) ) || (!secret_mode && !digitalRead(UPDATE_PIN))){
    update();
  }//*/
  if(digitalRead(STOP_ALARM_PIN) && music.is_active()){
    music.stop();
  }
  if(!digitalRead(TEST_ALARM_PIN) && !music.is_active() && !secret_mode){
    music.start();
  }
  if(!digitalRead(SOFT_RESET_PIN)){
    reset();
  }
  if(Serial.available()){
    if(Serial.read()=='D'){
      debug();
    }
  }
  music.run(); 
  delay(10);
}
