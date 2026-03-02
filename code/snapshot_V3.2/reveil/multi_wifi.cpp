#include "multi_wifi.h"

int WIFI_manager::try_connect() {
    // Reset to start if we are at the end or out of bounds
    if (wifi_id >= wifi_len) wifi_id = 0;

    for (int i = 0; i < wifi_len; i++) {
        if (try_one_connect()) {
            return true; // Success! wifi_id is now at the working index
        }
        wifi_id++;
        if (wifi_id >= wifi_len) wifi_id = 0; // Wrap around
    }
    return false; // None of the networks worked
}

int WIFI_manager::try_one_connect(){
  WiFi.disconnect(true); // Shut down WiFi fully
    WiFi.mode(WIFI_STA);   // Explicitly set Station mode
    delay(100);
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
