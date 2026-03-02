#ifndef LOCAL_PARAMS_H
#define LOCAL_PARAMS_H
#include "multi_wifi.h"

#define Ical_URL "your Ical URL"
inline void define_wifi(WIFI_manager* WIFI_ls){ // from 1 to 4 wifi lines 
    WIFI_ls->add_wifi("your WIFI SSID 1","your WIFI PSW 1");
    WIFI_ls->add_wifi("your WIFI SSID 2","your WIFI PSW 2");}

#endif
