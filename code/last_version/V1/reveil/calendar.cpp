#include "calendar.h"

// --- Configuration WiFi ---
const char* ssid = "your WIFI SSID";
const char* password = "your WIFI Password";





// --- Configuration du temps (NTP) ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; 
const int   daylightOffset_sec = 00; //0

#define ONE_HOUR 3600 // From the provided header

time_t parseCustomTime_LocalToUTC_Millis(String timeString, int32_t localOffsetSec) {
  struct tm tm_struct = {0}; // Initialize all fields to zero
  
  // Example timeString format: 20251013T064032
  
  // 1. Manual Parsing: Extract broken-down time components (assuming local time)
  tm_struct.tm_year = timeString.substring(0,4).toInt(); // e.g., 2025
  tm_struct.tm_mon  = timeString.substring(4,6).toInt(); // e.g., 10
  tm_struct.tm_mday = timeString.substring(6,8).toInt(); // e.g., 13
  tm_struct.tm_hour = timeString.substring(9,11).toInt(); // e.g., 06
  tm_struct.tm_min  = timeString.substring(11,13).toInt(); // e.g., 40
  tm_struct.tm_sec  = timeString.substring(13,15).toInt(); // e.g., 32
  
  // 2. Adjust struct tm fields (standard C requirement)
  tm_struct.tm_year -= 1900; // Years since 1900 
  tm_struct.tm_mon  -= 1;    // Months since January
  tm_struct.tm_isdst = (localOffsetSec == 2 * ONE_HOUR) ? 1 : 0; 
  // NOTE: tm_isdst flag is primarily for mktime to determine if it should apply a DST correction, 
  // but since the local time is provided, we must *manually* correct for the offset.
  
  // 3. Convert Local Time struct to UTC Time_t 
  // This step MUST be done carefully based on how mktime is implemented in avr-libc.
  // The header suggests mktime interprets the struct as Local Time and applies 
  // the configured system time zone *if configured* (set_zone()). 
  // To avoid confusion with system-configured zone, we will use the local time property:
  
  time_t local_timestamp = mktime(&tm_struct); 
  
  // 4. Manual Correction for the Time Zone
  // The local_timestamp now represents the number of seconds since Jan 1 2000, 
  // corrected for the system's timezone.
  // To convert this Local Time (e.g., CET/CEST) back to true UTC, you subtract the offset.
  time_t utc_timestamp_sec = local_timestamp - localOffsetSec;
  
  // 5. Convert seconds to milliseconds
  // NOTE: time_t is a uint32_t, so be careful about overflow if you directly cast the result to a long long.
  // Since the result must be time_t, and your time_t is a uint32_t (seconds), 
  // you might need to return a different type for milliseconds, such as an unsigned long long or a struct.
  // Assuming you can return a 64-bit integer for milliseconds:
  
  uint64_t utc_timestamp_ms = (uint64_t)utc_timestamp_sec * 1000;
  
  return utc_timestamp_ms; // Return the time in milliseconds since 2000 UTC
}


time_t parseCustomTime(String timeString) {
  struct tm tm_struct = {0}; // Initialize all fields to zero
  
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
  
  // Set tm_isdst to -1 to force mktime() to determine if DST is in effect 
  // based on the date/time and the configured set_dst() rules.
  tm_struct.tm_isdst = -1; 
  
  // 3. Conversion to UTC Time_t 
  // mktime() interprets the tm_struct as local time and converts it to the 
  // system's internal UTC-based time_t, applying DST rules automatically.
  time_t utc_timestamp_sec = mktime(&tm_struct); 
  
  // 4. Return the time_t value (seconds since 2000 UTC).
  return utc_timestamp_sec;
}




event::event(){}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool initLocalTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

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
/*
void print_time(String msg,time_t time_code){
  struct tm timeinfo;
  char time_buffer[64];
  strftime(time_buffer, 64, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.print("Heure actuelle ESP32: ");
  Serial.println(time_buffer);
}
//*/
//////////

String calendar::get_data_from_event(String id, int N){
  long Pos = 0;
  for(int i =0;i<N;i++) Pos = ical_event.indexOf(id,Pos+1);
  if(Pos!=-1){
    return ical_event.substring(ical_event.indexOf(":",Pos)+1,ical_event.indexOf("\r",Pos));
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

void calendar::generate_events(){
  event_number = 0;
  Serial.println("reset all events");
  Serial.print("generating events...");
  time_t now = time(nullptr);
  struct tm now_tm;
  getLocalTime(&now_tm); 

  const time_t aDay = 24 * 3600;
  time_t max_time;
  
  int event_start_pos = Ical.indexOf("BEGIN:VEVENT");
  int event_end_pos;
  time_t event_start_time;
  
  while(event_start_pos>=0){// generation des event recurcifs 
    max_time = now + 7*aDay;
    event_end_pos = Ical.indexOf("END:VEVENT",event_start_pos)+10;
    ical_event = Ical.substring(event_start_pos,event_end_pos);

    if(get_data_from_event("STATUS",1).equals("CONFIRMED")&&(ical_event.indexOf("RRULE")!=-1)) {
      event_start_time = parseCustomTime(get_data_from_event("DTSTART",1));
      //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("-------------------------------------\n"+ical_event); //debug
      while(event_start_time<now){
        event_start_time+=aDay;
      }
      if(ical_event.indexOf("UNTIL=")!=-1){
        String RRule = get_data_from_event("RRULE",1);
        String until = RRule.substring(RRule.indexOf("UNTIL=")+6);
        time_t until_t = parseCustomTime(until);
        if(until_t<max_time) max_time = until_t;
        //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("until saw at "+String(until_t)); //debug
      }
      String name = get_data_from_event("SUMMARY",1);
      String UID = get_data_from_event("UID",1);
      while(event_start_time<max_time){
        //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("tryed "+String(event_start_time)); //debug
        bool exDATE = false;
        int n = 1;
        String EXDATE = get_data_from_event("EXDATE",1);
        while(!(EXDATE.equals(""))){
          if (event_start_time == parseCustomTime(EXDATE)) exDATE = true;
          n++;
          EXDATE = get_data_from_event("EXDATE",n);
        }
        if(!exDATE){
          all_events[event_number].name = name;
          all_events[event_number].UID = UID;
          all_events[event_number].start = event_start_time;
          event_number++;
          //if(get_data_from_event("SUMMARY",1).equals("reveil")) Serial.println("added at "+String(event_start_time)); //debug
        }//else Serial.println("there is an exeption");                                                                 //debug
        event_start_time += aDay;
      }
    }
    
    event_start_pos = Ical.indexOf("BEGIN:VEVENT", event_start_pos+1); // recherche du prochain evenement.
  }


  event_start_pos = Ical.indexOf("BEGIN:VEVENT");
  time_t event_recurence_time;
  int search_event_id = 0;
  
  while(event_start_pos>=0){   // generation des event uniques et spéciaux
    max_time = now + 7*aDay;
    event_end_pos = Ical.indexOf("END:VEVENT",event_start_pos)+10;
    ical_event = Ical.substring(event_start_pos,event_end_pos);

    if(get_data_from_event("STATUS",1).equals("CONFIRMED")&&(ical_event.indexOf("RRULE")==-1)) {
      event_start_time = parseCustomTime(get_data_from_event("DTSTART",1));
      String name = get_data_from_event("SUMMARY",1);
      String UID = get_data_from_event("UID",1);
      if(ical_event.indexOf("RECURRENCE-ID")==-1){
        if(event_start_time>now && max_time>event_start_time){
          all_events[event_number].name = name;
          all_events[event_number].UID = UID;
          all_events[event_number].start = event_start_time;
          event_number++;
        }
      }else{
        event_recurence_time = parseCustomTime(get_data_from_event("RECURRENCE-ID",1));
        if(event_recurence_time>now && max_time>event_recurence_time){
          while(!(all_events[search_event_id].UID.equals(UID)&&all_events[search_event_id].start == event_recurence_time)){
            search_event_id++;
            if (search_event_id>event_number){
              break;
            }
          }
          all_events[search_event_id].name = name;
          all_events[search_event_id].UID = UID;
          all_events[search_event_id].start = event_start_time;
        }
      }
    }
    
    event_start_pos = Ical.indexOf("BEGIN:VEVENT", event_start_pos+1); // recherche du prochain evenement.
  }
  Serial.println("OK");
  Serial.println("we have "+String(event_number)+" events for the next week.");
}









bool calendar::update(){
  // Initialisation de l'heure NTP
  Serial.println("");
  if(WiFi.status() != WL_CONNECTED) if(!init_WiFi()) return false ;
  bool ret = initLocalTime();
  if (!ret) ret = initLocalTime();
  if (!ret) ret = initLocalTime();
  if (!ret) return false;
  
  ret = update_Ical();
  if (!ret) ret = update_Ical();
  if (!ret) ret = update_Ical();
  if (!ret) return false;
  generate_events();
  return true;
}

time_t calendar::next_event_named(String event_name){
  time_t now = time(nullptr);
  time_t least_time = 32503680000;
  for(int i = 0;i<event_number;i++){
    if(least_time>(all_events[i].start) && now < (all_events[i].start) && (all_events[i].name).equals(event_name)){
      least_time=all_events[i].start;
    }
  }
  return least_time;
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

void calendar::debug_inject(){
  Serial.println("from "+String(event_number)+" events we have : ");
  for(int i = 0;i<event_number;i++ ){
    if((all_events[i].name).equals("reveil")){
      Serial.println("-------------------------");
      Serial.println(all_events[i].UID);
      Serial.println(String(all_events[i].start));
    }
  }
}

calendar::calendar(char* icalUrl_input){
  icalUrl = icalUrl_input;
}
