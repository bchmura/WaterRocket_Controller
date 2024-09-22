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
#include "communications.hpp"

void setupLogging(int LogLevel);
void setupEspNow();
void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void sendMsg();
void loopCheckButtons();
void setupButtons();
void sendPing();

uint8_t launcher_mac[] = {0xE4, 0x65, 0xB8, 0x14, 0x77, 0x14};
uint8_t controller_mac[] = {0xd0,0xef,0x76,0x45,0x01,0x08};

throbbingButtonState throbbingButton;
ControllerDisplay display;
ApplicationState appState;

Bounce2::Button fuelButton = Bounce2::Button();
Bounce2::Button launchButton = Bounce2::Button();

bool lastRequestToLauncherForActuator = false;
bool lastRequestToLauncherForSolenoid = false;

void setupDisplay() {
   display.Setup();
};

void sendPing() {
   Log.infoln("Ping sending");
   Ping ping;
   ping.pingId = 2222;
   uint8_t message[sizeof(ping)];
   memcpy(&message[0], &ping, sizeof(ping));
   Log.infoln("Ping created");
   ESPNow.send_message(launcher_mac, message, sizeof(message));
   Log.infoln("Ping sent");
};

void setup() {
   Serial.begin(9600);
   delay(2000);
   Serial.println("BR-L starting");
   ///setupLogging(LOG_LEVEL_VERBOSE);
   setupLogging(LOG_LEVEL_VERBOSE);
   setupButtons();
   setupDisplay();
   setupEspNow();
   //taskManager.schedule(repeatMillis(20000), sendPing);
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
   taskManager.runLoop();
   loopCheckButtons();
   display.UpdateAppStatus(appState);
   display.RunLoop();
   //delay(2000);
}

void setupEspNow() {
   uint8_t baseMac[6];
   std::string macAddressString;
   esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
   Log.infoln("Wifi station interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);
   esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
   Log.infoln("Wifi softAP interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);
   esp_read_mac(baseMac, ESP_MAC_ETH);
   Log.infoln("Ethernet interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);
   esp_read_mac(baseMac, ESP_MAC_BT);
   Log.infoln("Bluetooth interface: %x:%x:%x:%x:%x:%x", baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);
   WiFi.mode(WIFI_MODE_STA);
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
   char macStr[18];
   snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
            mac_addr[5]);

  // Log.infoln("Message received from %s",macStr);

   uint8_t type = data[0];
   uint8_t subType = data[1];
 //  Log.infoln("Data length is set at %d", data_len);
 //  Log.infoln("Type detected as %X", type);
 //  Log.infoln("SubType detected as %X", subType);

   // for (int x = 0; x < data_len; x++) {
   //    Serial.print(data[x], HEX);
   //    Serial.print(" ");
   // }

   switch (type) {
      case 0x01: {
         // This is a SystemStatus struct
         memcpy(&appState, &data[2], sizeof(appState));
         //Log.infoln("Rocket Pressure at %d", appState.rocketPressurePsi);
         break;
      }
      case PingAckType: {
         PingAck response;
         memcpy(&response, &data[0], sizeof(response));
         Log.infoln("PingAck received: %d", response.pingId);
         break;
      }
      default: {
         Serial.println("Unknown message type");
         break;
      }
   }



}

void setupButtons() {

   fuelButton.attach(BUTTON_FUEL_PIN, INPUT_PULLUP);
   fuelButton.setPressedState(LOW);
   fuelButton.interval(5);

   launchButton.attach(BUTTON_LAUNCH_PIN, INPUT_PULLDOWN);
   launchButton.setPressedState(HIGH);
   launchButton.interval(5);

   Log.infoln("Setup buttons complete");
}


void sendCmdToggleSolenoid(bool newState) {
   Log.infoln("sendCmdToggleSolenoid to %b", newState);
   lastRequestToLauncherForSolenoid = newState;
   CmdToggleSolenoid command;
   command.newStateRequested = newState;
   uint8_t message[sizeof(command)];
   memcpy(&message[0], &command, sizeof(command));
   Log.infoln("Command created");
   ESPNow.send_message(launcher_mac, message, sizeof(message));
   Log.infoln("Command sent");

};

void sendCmdToggleActuator(bool newState) {

   Log.infoln("sendCmdToggleActuator to %b", newState);
   lastRequestToLauncherForActuator = newState;
   CmdToggleActuator command;
   command.newStateRequested = newState;
   uint8_t message[sizeof(command)];
   memcpy(&message[0], &command, sizeof(command));
   Log.infoln("Command created");
   ESPNow.send_message(launcher_mac, message, sizeof(message));
   Log.infoln("Command sent");

};

void loopCheckButtons()
{

   fuelButton.update();
   if (fuelButton.changed() && fuelButton.pressed()) {
      Log.infoln("##################### fuel button was pressed");
      //appState.isSolendoidOpen = solenoidControl.toggle();
      //appState.isDirty = true;
      if (lastRequestToLauncherForSolenoid) {
         sendCmdToggleSolenoid(false);
      } else {
         sendCmdToggleSolenoid(true);
      }

   } else if (fuelButton.changed()) {
      Log.infoln("##################### fuel button was unpressed");
   }

   launchButton.update();
   if (launchButton.changed() && launchButton.pressed()) {
      Log.infoln("##################### launch button was pressed");
      //appState.isSolendoidOpen = solenoidControl.toggle();
      //appState.isDirty = true;
      if (lastRequestToLauncherForActuator) {
         sendCmdToggleActuator(false);
      } else {
         sendCmdToggleActuator(true);
      }


   } else if (launchButton.changed()) {
      Log.infoln("##################### launch button was unpressed");
   }
}

