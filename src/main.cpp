#include <Arduino.h>
#include <ArduinoLog.h>
#include <Bounce2.h>
#include <ESPNowW.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPI.h>
#include <TaskManagerIO.h>
#include <cstdlib>
#include "Models/ControllerDisplay.hpp"

#include "Models/ApplicationStatus.hpp"


void setupLogging(int LogLevel);
void setupEspNow();
void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void sendMsg();

uint8_t launcher_mac[] = {0xE4, 0x65, 0xB8, 0x14, 0x77, 0x14};

throbbingButtonState throbbingButton;
ControllerDisplay display;

void setupDisplay() {
   display.Setup();
};

void setup() {
   Serial.begin(9600);
   delay(2000);
   Serial.println("BR-L starting");
   ///setupLogging(LOG_LEVEL_VERBOSE);
   setupLogging(LOG_LEVEL_VERBOSE);
   setupDisplay();
   setupEspNow();
   Log.infoln("main setup complete");
}

ApplicationState getMockAppState() {
   ApplicationState app;
   app.isDirty = true;
   app.isLauncherVertical = rand() % 2;
   app.isSolendoidOpen  = rand() % 2;
   app.isRocketDetected  = rand() % 2; // not implemented yet
   app.resevoirPressurePsi = 67;
   app.resevoirPressureVoltage = 328;
   app.rocketPressurePsi = 24;
   app.rocketPressureVoltage = 200;
   app.isLaunchButtonPressed  = rand() % 2;
   app.isPressurizeButtonPressed  = rand() % 2;

   return app;

};

void loop() {
   ApplicationState app_state = getMockAppState();
   display.UpdateAppStatus(app_state);
   display.RunLoop();
   delay(2000);
}

void setupEspNow() {
   uint8_t baseMac[6];
   std::string macAddressString;

   // Get MAC address of the WiFi station interface
   esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
   Log.infoln("Wifi station interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);

   // Get the MAC address of the Wi-Fi AP interface
   esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
   Log.infoln("Wifi softAP interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);

   // Get the MAC address of the Ethernet interface
   esp_read_mac(baseMac, ESP_MAC_ETH);
   Log.infoln("Ethernet interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);

   // Get the MAC address of the Bluetooth interface
   esp_read_mac(baseMac, ESP_MAC_BT);
   Log.infoln("Bluetooth interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);

   WiFi.mode(WIFI_MODE_STA);
   //ESPNow.set_mac(LAUNCHER_MAC);
   WiFi.disconnect();
   ESPNow.init();
   ESPNow.reg_recv_cb(onRecv);
   ESPNow.add_peer(launcher_mac);
   display.SetEspNowActive(true);
}

void setupLogging(const int LogLevel) {
   Log.begin(LogLevel, &Serial);
   Log.infoln("Logging started at level %d", LogLevel);
}


void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
   // char macStr[18];
   // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
   //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
   //          mac_addr[5]);

 // uint8_t dataCopy[data_len];
  //memcpy(dataCopy, data, data_len);
   //ApplicationState appState;
   //memcpy(&appState, &data[2], sizeof(appState));
   //Log.infoln("Pressure at %T", appState.isLauncherVertical);

   uint8_t type = data[0];
   uint8_t subType = data[1];
   Log.infoln("Data length is set at %d", data_len);
   Log.infoln("Type detected as %X", type);
   Log.infoln("SubType detected as %X", subType);


   ApplicationState appState;
   memcpy(&appState, &data[2], sizeof(appState));
   Log.infoln("Pressure at %T", appState.isLauncherVertical);

   for (int x = 0; x < data_len; x++) {
      Serial.print(data[x], HEX);
      Serial.print(" ");
   }

/*   switch (type) {
      case 0x01: {  // This is a SystemStatus struct
         ApplicationState appState;
         memcpy(&appState, &data[2], sizeof(appState));
         Log.infoln("Pressure at %T", appState.isLauncherVertical);
         break;
      }

      default: {
         Serial.println("Unknown message type");
         break;
      }
   }
*/

}
