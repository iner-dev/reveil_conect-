#ifndef CALENDAR_H
#define CALENDAR_H
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

#define max_event 255
time_t parseCustomTime(String timeString);
char* to_char(String str);

class event{
  public:
  event();
  String name;
  String UID;
  time_t start;
  private:
};

class calendar{
  public:
  
  calendar(char* icalUrl_input );
  bool update();
  time_t next_event_named(String event_name);
  void debug_inject();
  
  private:

  char*icalUrl;
  bool init_WiFi();
  void generate_events();
  bool update_Ical();
  String Ical="";
  String ical_event;
  event all_events[max_event];
  int event_number;
  String get_data_from_event(String id,int N);
};

#endif
