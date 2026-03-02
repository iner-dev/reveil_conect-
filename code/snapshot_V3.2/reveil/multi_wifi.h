#ifndef MULTI_WIFI_H 
#define MULTI_WIFI_H
#include <Arduino.h>
#include <WiFi.h>
#define max_wifi 4

struct wifi{
  String ssid;
  String PSW;
};

class WIFI_manager{
public:
    bool try_connect();
    void add_wifi(String SSID, String PSW);
private:
    bool try_one_connect();
    wifi wifi_list[max_wifi];
    int wifi_id = 0;
    int wifi_len = 0;
};

#endif
