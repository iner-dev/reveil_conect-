#include "calendar.h"

// --- Configuration WiFi ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PSW;

// --- Configuration du temps (NTP) ---
const char* ntpServer = NTP_SERVER_ADDRESS;
char* defaultTimezone = TIME_ZONE;

#define ONE_HOUR 3600 // From the provided header

time_t parseCustomTime(String timeString) {
  //Serial.println("parsing "+timeString);
  struct tm tm_struct = {0}; // Initialize all fields to zero

  if (timeString.substring(0,2) == "TZ"){
    tm_struct.tm_isdst = -1; 
    timeString = timeString.substring(timeString.indexOf(":")+1);
  }else{
    tm_struct.tm_isdst = 0;
  }
  // 1. Manual Parsing: Extract broken-down time components (Local Time)
  tm_struct.tm_year = timeString.substring(0,4).toInt(); 
  tm_struct.tm_mon  = timeString.substring(4,6).toInt(); 
  tm_struct.tm_mday = timeString.substring(6,8).toInt(); 
  tm_struct.tm_hour = timeString.substring(9,11).toInt(); 
  tm_struct.tm_min  = timeString.substring(11,13).toInt(); 
  tm_struct.tm_sec  = timeString.substring(13,15).toInt(); 
  
  // 2. Adjust struct tm fields (Standard C requirement)
  tm_struct.tm_year -= 1900; // Years since 1900 
  tm_struct.tm_mon  -= 1;    // Months since January
  
  
  
  // 3. Conversion to UTC Time_t 
  // mktime() interprets the tm_struct as local time and converts it to the 
  // system's internal UTC-based time_t, applying DST rules automatically.
  time_t utc_timestamp_sec = mktime(&tm_struct); 
  if(timeString.substring(timeString.length()-1)=="Z") utc_timestamp_sec += 3600;
  
  
  // 4. Return the time_t value (seconds since 2000 UTC).
  return utc_timestamp_sec;
}




event::event(){}

void event::serial_dump(){
  Serial.println("name  = "+name);
  Serial.println("UID   = "+UID);
  Serial.println("start = "+String(start));
  Serial.println("end   = "+String(end));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool initLocalTime() {
  configTzTime( defaultTimezone, ntpServer); 

  Serial.println("Synchronisation de l'heure via NTP...");
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Echec de la recuperation de l'heure. Verifiez le WiFi ou le NTP.");
    return false;
  }
  char time_buffer[64];
  strftime(time_buffer, 64, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.print("Heure actuelle ESP32: ");
  Serial.println(time_buffer);
  return true;
}
//////////

String calendar::get_data_from_event(String id, int N){
  long Pos = 0;
  for(int i =0;i<N;i++) Pos = ical_event.indexOf(id,Pos+1);
  if(Pos!=-1){
    return ical_event.substring(Pos+id.length()+1,ical_event.indexOf("\r",Pos));
  }else{
    return "";
  }
}

//////////

bool calendar::update_Ical(){
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Erreur: WiFi deconnecte.");
    return false;
  }
  
  

  HTTPClient http;
  Serial.print("Telechargement de l'iCal... ");
  
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(icalUrl); 
  
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Erreur HTTP: %d\n", httpCode);
    http.end();
    return false;
  }

  Ical = http.getString();
  http.end();
  Serial.println("OK.");
  return true;
}

//////////

void calendar::generate_recurcive_event_betwin(time_t min_time,time_t max_time){
  time_t event_start_time;
  String RRule;
  String until;
  String name;
  String UID;
  bool exDATE;
  int n;
  String EXDATE;
  time_t event_end_time;
  
  if(get_data_from_event("STATUS",1).equals("CONFIRMED")&&(ical_event.indexOf("RRULE")!=-1)) {
    event_start_time = parseCustomTime(get_data_from_event("DTSTART",1));
    event_end_time = parseCustomTime(get_data_from_event("DTEND",1));
    //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("-------------------------------------\n"+ical_event); //debug
    while(event_start_time<min_time){
      event_start_time+=aDay;
      event_end_time+=aDay;
    }
    if(ical_event.indexOf("UNTIL=")!=-1){
      RRule = get_data_from_event("RRULE",1);
      until = RRule.substring(RRule.indexOf("UNTIL=")+6);
      time_t until_t = parseCustomTime(until);
      if(until_t<max_time) max_time = until_t;
      //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("until saw at "+String(until_t)); //debug
    }
    name = get_data_from_event("SUMMARY",1);
    UID = get_data_from_event("UID",1);
    while(event_start_time<max_time){
      //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("tryed "+String(event_start_time)); //debug
      exDATE = false;
      n = 1;
      EXDATE = get_data_from_event("EXDATE",1);
      while(!(EXDATE.equals(""))){
        if (event_start_time == parseCustomTime(EXDATE)) exDATE = true;
        n++;
        EXDATE = get_data_from_event("EXDATE",n);
      }
      if(!exDATE){
        all_events[event_number].name = name;
        all_events[event_number].UID = UID;
        all_events[event_number].start = event_start_time;
        all_events[event_number].end = event_end_time;
        if(ical_event.indexOf("DESCRIPTION")!=-1) all_events[event_number].description = get_data_from_event("DESCRIPTION",1);
        event_number++;
        //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("added at "+String(event_start_time)); //debug
      }//else Serial.println("there is an exeption");                                                                 //debug
      event_start_time += aDay;
      event_end_time+=aDay;
    }
  }
}

void calendar::generate_single_event_betwin(time_t min_time,time_t max_time){
  time_t event_start_time;
  time_t event_end_time;
  String name;
  String UID;
  int search_event_id = 0;
  time_t event_recurence_time;
  
  if(get_data_from_event("STATUS",1).equals("CONFIRMED")&&(ical_event.indexOf("RRULE")==-1)) {
      event_start_time = parseCustomTime(get_data_from_event("DTSTART",1));
      name = get_data_from_event("SUMMARY",1);
      UID = get_data_from_event("UID",1);
      event_end_time = parseCustomTime(get_data_from_event("DTEND",1));
      
      if(ical_event.indexOf("RECURRENCE-ID")==-1){
        if(event_start_time>min_time && max_time>event_start_time){
          all_events[event_number].name = name;
          all_events[event_number].UID = UID;
          all_events[event_number].start = event_start_time;
          all_events[event_number].end = event_end_time;
          if(ical_event.indexOf("DESCRIPTION")!=-1) all_events[event_number].description = get_data_from_event("DESCRIPTION",1);
          event_number++;
        }
      }else{
        event_recurence_time = parseCustomTime(get_data_from_event("RECURRENCE-ID",1));
        if(event_recurence_time>min_time && max_time>event_recurence_time){
          while( (search_event_id<event_number)  &&  !(  all_events[search_event_id].UID.equals(UID)  &&  all_events[search_event_id].start == event_recurence_time)  ){
            search_event_id++;
          }
          if (search_event_id == event_number)return;
        }else{
          search_event_id = event_number;
          event_number++;
        }
        all_events[search_event_id].name = name;
        all_events[search_event_id].UID = UID;
        all_events[search_event_id].start = event_start_time;
        all_events[search_event_id].end = event_end_time;
        if(ical_event.indexOf("DESCRIPTION")!=-1) all_events[search_event_id].description = get_data_from_event("DESCRIPTION",1);
      }
    }
}

void calendar::generate_events(){
  time_t now = time(nullptr);
  struct tm now_tm;
  time_t max_time = now + SCAN_LENGHT*aDay;
  int event_start_pos = Ical.indexOf("BEGIN:VEVENT");
  int event_end_pos;

  
  event_number = 0;
  Serial.println("reset all events");
  Serial.print("generating events...");
  getLocalTime(&now_tm); 
  
  while(event_start_pos>=0){// generation des event recurcifs 
    event_end_pos = Ical.indexOf("END:VEVENT",event_start_pos)+10;
    ical_event = Ical.substring(event_start_pos,event_end_pos);
    generate_recurcive_event_betwin(now,max_time);
    event_start_pos = Ical.indexOf("BEGIN:VEVENT", event_start_pos+1); // recherche du prochain evenement.
    //Serial.println(event_number);
  }


  event_start_pos = Ical.indexOf("BEGIN:VEVENT");
  
  while(event_start_pos>=0){   // generation des event uniques et spéciaux
    event_end_pos = Ical.indexOf("END:VEVENT",event_start_pos)+10;
    ical_event = Ical.substring(event_start_pos,event_end_pos);
    
    generate_single_event_betwin(now-900,max_time);
    
    event_start_pos = Ical.indexOf("BEGIN:VEVENT", event_start_pos+1); // recherche du prochain evenement.
  }
  Serial.println("OK");
  Serial.println("we have "+String(event_number)+" events for the next week.");
}


bool calendar::update(){
  // Initialisation de l'heure NTP
  Serial.println("");
  if(WiFi.status() != WL_CONNECTED) if(!init_WiFi()) return false ;
  bool ret = update_Ical();
  if (!ret) ret = update_Ical();
  if (!ret) ret = update_Ical();
  if (!ret) return false;
  
  ret = initLocalTime();
  if (!ret) ret = initLocalTime();
  if (!ret) ret = initLocalTime();
  if (!ret) return false;

  #ifdef AUTO_DISCONNECT
  WiFi.disconnect();
  Serial.println("WiFi Disconnecté");
  #endif
  
  generate_events();
  return true;
}

event calendar::next_event_named_after(String event_name,time_t T){
  int least_i = max_event-1;
  int i;
  all_events[least_i].start = MAX_START_TIMESTAMP;
  all_events[least_i].end = MAX_END_TIMESTAMP;
  for(i = 0;i<event_number;i++){
    if(all_events[least_i].start>(all_events[i].start) && T < (all_events[i].start) && (all_events[i].name).equals(event_name)){
      least_i = i;
    }
  }
  return 
  all_events[least_i];
}

bool calendar::init_WiFi(){
  long T0 = millis();
  // Connexion WiFi
  Serial.print("Connexion a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && (millis()-T0)<10000) {
    delay(500);
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nWiFi connecte !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
  }else Serial.println("\nWiFi time out error");
  return WiFi.status() == WL_CONNECTED;
}

void calendar::serial_dump(){
  Serial.println("from "+String(event_number)+" events we have : ");
  for(int i = 0;i<event_number;i++ ){
    Serial.println("-------------------------");
    all_events[i].serial_dump();
  }
}

calendar::calendar(char* icalUrl_input){
  icalUrl = icalUrl_input;
}
