/**
 * OnDemandConfigPortal.ino
 * example of running the configPortal AP manually, independantly from the captiveportal
 * trigger pin will start a configPortal AP for 120 seconds then turn it off.
 * 
 */
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <NTPClient.h>

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN 0

int timeout = 120; // seconds to run for

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("\n Starting");
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
}

void loop() {
  // is configuration portal requested?
  if ( digitalRead(TRIGGER_PIN) == LOW) {
    Serial.println("Button Pressed\n");
    WiFiManager wm;    
    //reset settings - for testing
    //wm.resetSettings();
  
    // set configportal timeout
    wm.setConfigPortalTimeout(timeout);

    if (!wm.startConfigPortal("SpirulinaServer")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected... :)");
  }

  // put your main code here, to run repeatedly:


}
