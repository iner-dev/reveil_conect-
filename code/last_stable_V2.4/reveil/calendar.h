#ifndef CALENDAR_H
#define CALENDAR_H
#include <Arduino.h>
#include "software_parameters.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>


time_t parseCustomTime(String timeString);
char* to_char(String str);
const time_t aDay = 24 * 3600;

class event{
  public:
  event();
  void serial_dump();
  String name;
  String UID;
  time_t start;
  time_t end;
  private:
};

class calendar{
  public:
  
  calendar(char* icalUrl_input );
  bool update();
  event next_event_named_after(String event_name,time_t T);
  void serial_dump();
  
  private:

  char*icalUrl;
  bool init_WiFi();
  void generate_events();
  bool update_Ical();
  String Ical="";
  String ical_event;
  void generate_recurcive_event_betwin(time_t min_time,time_t max_time);
  void generate_single_event_betwin(time_t min_time,time_t max_time);
  event all_events[max_event];
  int event_number;
  String get_data_from_event(String id,int N);
};

#endif
