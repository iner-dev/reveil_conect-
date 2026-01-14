#ifndef SOFTWARE_PARAMS_H
#define SOFTWARE_PARAMS_H


// reveil

#define Ical_URL "your ical URL"
#define UPDATE_MIN_DELAY_NORMAL 21600   //  S
#define UPDATE_MIN_DELAY_ERROR    300   //  S
#define UPDATE_EVENT_LOCK_DELAY    60   //  S
#define UPDATE_GETTING_OFFSET       0   //  S
#define TEST_LIGHT_TIME             5   //  S
#define REFRESH_DELAY              10   //  ms
#define ERROR_LED_FREQUANCY       200   //  ms
#define SECRET_LED_FREQUANCY      500   //  ms

// WIFI parms

//*
#define WIFI_SSID "your Wifi SSID"
#define WIFI_PSW  "your Wifi PSW"
/*/
#define WIFI_SSID "RMDT"
#define WIFI_PSW  "iner_the_1"
//*/
#define AUTO_DISCONNECT // active l'arret automatique du WiFi lors qu'il ne se met pas a jours 

//calendar parms
#define max_event                    256
#define SCAN_LENGHT                    7 //Days
#define MAX_START_TIMESTAMP  32503680000 //S
#define MAX_END_TIMESTAMP    32503680001 //S

// song parms

#define NOTE_TIME 500
#define MAX_BOUCLE 10

#endif
