#ifndef SOFTWARE_PARAMS_H
#define SOFTWARE_PARAMS_H

#include "local_parameters.h"

// reveil

#define UPDATE_MIN_DELAY_NORMAL 21600   //  S
#define UPDATE_MIN_DELAY_ERROR    300   //  S
#define UPDATE_EVENT_LOCK_DELAY    60   //  S
#define UPDATE_GETTING_OFFSET       0   //  S
#define TEST_LIGHT_TIME             5   //  S
#define REFRESH_DELAY              10   //  ms
#define ERROR_LED_FREQUANCY       200   //  ms
#define SECRET_LED_FREQUANCY      500   //  ms

// time parms

#define NTP_SERVER_ADDRESS    "pool.ntp.org"
#define TIME_ZONE             "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

// WIFI parms

#define AUTO_DISCONNECT // active l'arret automatique du WiFi lors qu'il ne se met pas a jours 

//calendar parms
#define max_event                    256
#define SCAN_LENGHT                    7 //Days
#define MAX_START_TIMESTAMP  32503680000 //S
#define MAX_END_TIMESTAMP    32503680001 //S

// song parms

#define MAX_PLAY_TIME 300000
#define SONG_VOLUME 20 

#endif
