#include "multi_wifi.h"

int WIFI_manager::try_connect(){
    if(wifi_id<wifi_len && wifi_id>0) if(try_one_connect())return true;
    else wifi_id = 0;
    
    while(!try_one_connect() && wifi_id<wifi_len) wifi_id++;
    return (wifi_id!=wifi_len);
}

int WIFI_manager::try_one_connect(){
    long T0 = millis();
  // Connexion WiFi
  Serial.print("Connexion a ");
  Serial.println(wifi_list[wifi_id].ssid);
  WiFi.begin(wifi_list[wifi_id].ssid, wifi_list[wifi_id].PSW);

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

void WIFI_manager::add_wifi(String SSID, String PSW){
    if(wifi_len<max_wifi) {
        wifi_list[wifi_len].ssid = SSID;
        wifi_list[wifi_len].PSW = PSW;
        wifi_len++;
    }
}