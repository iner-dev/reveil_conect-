#include "calendar.h"
#include "song_runner.h"
#include "pin.h"
#include "software_parameters.h"

//#define DEBUG

calendar gcal(Ical_URL);
song_runner music;
event next_alarm_event;
event next_alert_event;
int alert_id=0;
time_t last_update;
time_t now;
time_t reboot_time = 3600;

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
  now = time(nullptr);
  if (updated){
    next_alarm_event = gcal.next_event_named_after("reveil",now-UPDATE_GETTING_OFFSET);
    next_alert_event = gcal.next_event_named_after("alert",now-UPDATE_GETTING_OFFSET);
    reboot_time = gcal.next_event_named_after("reboot",now-UPDATE_GETTING_OFFSET).start;
  }
  last_update = now;
  Serial.println("next event at "+String(next_alarm_event.end)+"UTC in "+String(next_alarm_event.end-now)+"sec");
  Serial.println("next alert at "+String(next_alert_event.start)+"UTC in "+String(next_alert_event.start-now)+"sec");
  Serial.println("next reboot at "+String(reboot_time)+"UTC in "+String(reboot_time-now)+"sec");
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
        case 'D' :
          gcal.serial_dump();
          break; 
        case 'E' :
          debug_mode = false;
          break; 
        case 'T' :
          next_alarm_event.start = now;
          next_alarm_event.end = now+TEST_LIGHT_TIME;
          Serial.println("Testing");
          break;
        case 'A' :
          next_alert_event.start = now+TEST_LIGHT_TIME;
          next_alert_event.end = now+TEST_LIGHT_TIME;
          next_alert_event.description = "0";
          Serial.println("Testing");
          break;
        case 'd' :
          WiFi.disconnect();
          Serial.println("disconnected");
          break; //*
        case 'S' :
          music.Alert(1);
          Serial.println("Speaker ON");
          break; 
        case 's' :
          music.End_Alert();
          music.stop();
          Serial.println("Speaker OFF");
          break; /**/
        case 'M' :
          digitalWrite(ERROR_LED_PIN,HIGH);
          Serial.println("error LED ON");
          break; 
        case 'm' :
          digitalWrite(ERROR_LED_PIN,LOW);
          Serial.println("error LED OFF");
          break; 
        case 'L' :
          analogWrite(LIGHT_PIN, 255);
          Serial.println("Light ON");
          break; 
        case 'l' :
          analogWrite(LIGHT_PIN, 0);
          Serial.println("Light OFF");
          break; 
        case 'R' :
          reset();
          break; 
        case 'U' :
          update();
          break; 
        case 'N' :
          Serial.println("next event at "+String(next_alarm_event.end)+"UTC in "+String(next_alarm_event.end-now)+"sec");
          Serial.println("next alert at "+String(next_alert_event.start)+"UTC in "+String(next_alert_event.start-now)+"sec");
          Serial.println("next reboot at "+String(reboot_time)+"UTC in "+String(reboot_time-now)+"sec");
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
  #ifdef SONG_SETUP_NEEDED
    song_setup();
  #endif
  
  Serial.begin(115200);
  pinMode(STOP_ALARM_PIN,INPUT_PULLUP);
  pinMode(TEST_ALARM_PIN,INPUT_PULLUP);
  pinMode(UPDATE_PIN,INPUT_PULLUP);
  pinMode(SOFT_RESET_PIN,INPUT_PULLUP);
  pinMode(ERROR_LED_PIN,OUTPUT);
  pinMode(LIGHT_PIN,OUTPUT);
  digitalWrite(ERROR_LED_PIN,LOW);
  delay(1000); 
  next_alarm_event.start = MAX_START_TIMESTAMP;
  next_alarm_event.end   = MAX_END_TIMESTAMP;

  
  #ifndef DEBUG
    while(!digitalRead(SOFT_RESET_PIN)) {
      delay(100);
      if(!digitalRead(TEST_ALARM_PIN) && !digitalRead(UPDATE_PIN)) debug();
    }
    digitalWrite(ERROR_LED_PIN,HIGH);
    
    update();
  
    Serial.println("Booted");
  #endif
  #ifdef DEBUG
    // initialisation de debogage 
    update();
  #endif
}

void loop() {
  #ifndef DEBUG
    now = time(nullptr);
    //*
    if(secret_mode){    // error led gestion
      digitalWrite(ERROR_LED_PIN,(int(millis()/SECRET_LED_FREQUANCY)%2==0));
      if(digitalRead(STOP_ALARM_PIN) && digitalRead(TEST_ALARM_PIN)){
        music.Alert(69);
        analogWrite(LIGHT_PIN, 255);
      }else{
        analogWrite(LIGHT_PIN, 0);
      }
    }else /**/
    if(!digitalRead(TEST_ALARM_PIN) && digitalRead(STOP_ALARM_PIN)){
      Serial.println("secret enable");
      secret_mode = true;
      music.stop();
    }else if(!updated){
      digitalWrite(ERROR_LED_PIN,(int(millis()/ERROR_LED_FREQUANCY)%2==0));
    }
  
    if(!music.is_active() && !secret_mode){
      if(next_alarm_event.end<now){  // alarm starting time
        next_alarm_event = gcal.next_event_named_after("reveil",now);
        Serial.println("waking up buddy");
        music.start();
        digitalWrite(LIGHT_PIN,HIGH);
      }else if(next_alarm_event.start < now){
        analogWrite(LIGHT_PIN, int(((now-next_alarm_event.start)*255)/(next_alarm_event.end-next_alarm_event.start)) );
      }else if(next_alert_event.start<now){
        if(next_alert_event.description!="") alert_id = next_alert_event.description.toInt();
        else alert_id = 0;
        music.Alert(alert_id);
        next_alert_event = gcal.next_event_named_after("alert",now);
      }
    }
    
    if(secret_mode && !digitalRead(UPDATE_PIN)){ // secret mod disabling 
      secret_mode = false;
      music.End_Alert();
      if (updated) next_alarm_event = gcal.next_event_named_after("reveil",now);
      while(!digitalRead(UPDATE_PIN))delay(100);
      digitalWrite(ERROR_LED_PIN,LOW);
      analogWrite(LIGHT_PIN, 0);
    }
  
    
    if(( ((  (((last_update+UPDATE_MIN_DELAY_NORMAL) <now)&&updated )||(((last_update+UPDATE_MIN_DELAY_ERROR)<now)&&!updated )  )&& ((next_alarm_event.start-UPDATE_EVENT_LOCK_DELAY)>now)) ) || (!secret_mode && !digitalRead(UPDATE_PIN))){  // updating 
      update();
    }//*/
  
    
    if(digitalRead(STOP_ALARM_PIN)&&!secret_mode){ // alarm stoping
      if (updated) next_alarm_event = gcal.next_event_named_after("reveil",now);
      if (music.is_active()) music.stop();
      analogWrite(LIGHT_PIN, 0);
    }
  
    
    if(!digitalRead(TEST_ALARM_PIN) && !music.is_active() && !secret_mode){
      next_alarm_event.start = now;
      next_alarm_event.end = now+TEST_LIGHT_TIME;
    }
    if(!digitalRead(SOFT_RESET_PIN) or now>reboot_time){
      reset();
    }
    if(Serial.available()){
      if(Serial.read()=='D'){
        debug();
      }
    }
    music.run(); 
  #endif
  #ifdef DEBUG
    // execution de debogage
    debug(); 
  #endif
  delay(REFRESH_DELAY);
}
