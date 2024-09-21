//
// Created by bill on 9/10/2024.
//

#include "ControllerDisplay.hpp"
#include <TaskManagerIO.h>

#include <Update.h>
#include <config.hpp>
#include <pitches.h>

ApplicationState application_state;
bool isNewAppState = false;
bool isButtonThrobbing = false;

extern throbbingButtonState throbbingButton;

Adafruit_SSD1306 oledDisplay1(SCREEN1_OLED_WIDTH, SCREEN1_OLED_HEIGHT, &Wire, SCREEN1_OLED_RESET_PIN);
Adafruit_SSD1306 oledDisplay2(SCREEN2_OLED_WIDTH, SCREEN2_OLED_HEIGHT, &Wire, SCREEN2_OLED_RESET_PIN);

CRGB leds[LED_TOTAL_LEDS];

ControllerDisplay::ControllerDisplay() { }

void ControllerDisplay::Setup() {
    SetupBuzzer();
    SetupDisplays();
    SetupLights();
}

void ControllerDisplay::UpdateAppStatus(ApplicationState newAppState) {
    //if (newAppState != application_state) {
        application_state = newAppState;
        lastAppUpdateTime = std::chrono::high_resolution_clock::now();
        //Log.infoln("new app status detected");
        isNewAppState = true;
  //  }
}

void ControllerDisplay::RunLoop() {
   // Log.infoln("run display loop");
    UpdateDisplays();
    UpdateLights();
    UpdateSound();
    isNewAppState = false; // reset it after reading
}

void ControllerDisplay::UpdateDisplays() {

   // Log.infoln("Starting Updating display");
    //if (! isNewAppState) return;

   // Log.infoln("Updating display");

    int cx;
    char rocketVoltsText[5];
    char rocketPsiText[4];
    char reservoirVoltsText[5];
    char reservoirPsiText[4];

    cx = snprintf(rocketPsiText, 4, "%03d",application_state.rocketPressurePsi);
    cx = snprintf(reservoirPsiText, 4, "%03d",application_state.resevoirPressurePsi);
    cx = snprintf(rocketVoltsText, 5, "%d",application_state.rocketPressureVoltage);
    cx = snprintf(reservoirVoltsText, 5, "%d",application_state.resevoirPressureVoltage);

    // Log.infoln("Updating display %s", rocketPsiText);
    // Log.infoln("Updating display %s", rocketVoltsText);
    // Log.infoln("Updating display %s", reservoirPsiText);
    // Log.infoln("Updating display %s", reservoirVoltsText);

    oledDisplay1.clearDisplay();
    oledDisplay1.setTextWrap(false);
    oledDisplay1.setTextColor(SSD1306_WHITE);
    oledDisplay1.setTextSize(2);
    oledDisplay1.setTextWrap(false);
    oledDisplay1.setCursor(1, 1);
    oledDisplay1.print("Rocket");
    oledDisplay1.setTextSize(1);
    oledDisplay1.setCursor(100, 1);
    oledDisplay1.print(rocketVoltsText);
    oledDisplay1.setTextSize(6);
    oledDisplay1.setCursor(1, 20);
    oledDisplay1.print(rocketPsiText);
    oledDisplay1.setTextSize(2);
    oledDisplay1.setCursor(111, 17);
    oledDisplay1.print("P");
    oledDisplay1.setCursor(111, 32);
    oledDisplay1.print("S");
    oledDisplay1.setCursor(111, 48);
    oledDisplay1.print("I");
    oledDisplay1.display();

    oledDisplay2.clearDisplay();
    oledDisplay2.setTextWrap(false);
    oledDisplay2.setTextColor(SSD1306_WHITE);
    oledDisplay2.setTextSize(2);
    oledDisplay2.setTextWrap(false);
    oledDisplay2.setCursor(1, 1);
    oledDisplay2.print("Supply");
    oledDisplay2.setTextSize(1);
    oledDisplay2.setCursor(100, 1);
    oledDisplay2.print(reservoirVoltsText);
    oledDisplay2.setTextSize(6);
    oledDisplay2.setCursor(1, 20);
    oledDisplay2.print(reservoirPsiText);
    oledDisplay2.setTextSize(2);
    oledDisplay2.setCursor(111, 17);
    oledDisplay2.print("P");
    oledDisplay2.setCursor(111, 32);
    oledDisplay2.print("S");
    oledDisplay2.setCursor(111, 48);
    oledDisplay2.print("I");
    oledDisplay2.display();

    //Log.infoln("Update display completed");
}

void ControllerDisplay::UpdateLights() {

    //Log.infoln("Updating lights");
    FastLED.setBrightness(LED_BRIGHTNESS);


    // ESP NOW
    if (isEspNowActive) {
        leds[LED_FOR_ESPNOWACTIVE] = CRGB::Green;
    } else {
        leds[LED_FOR_ESPNOWACTIVE] = CRGB::Red;
    }

    // last update
    // Calculate the elapsed time in milliseconds
    std::chrono::duration<double, std::milli> elapsed = std::chrono::high_resolution_clock::now() - lastAppUpdateTime;

    if (elapsed < std::chrono::milliseconds(500)) {
        leds[LED_FOR_UPDATE] = CRGB::Green;
    } else if (elapsed < std::chrono::milliseconds(1000)) {
        leds[LED_FOR_UPDATE] = CRGB::Yellow;
    } else {
        leds[LED_FOR_UPDATE] = CRGB::Red;
    }

    //launcher Vertical
    if (application_state.isLauncherVertical) {
        leds[LED_FOR_LAUNCHERVERTICAL] = CRGB::Green;
    } else {
        leds[LED_FOR_LAUNCHERVERTICAL] = CRGB::Red;
    }

    //Rocket pressure
    if (application_state.rocketPressurePsi < 50) {
        leds[LED_FOR_ROCKETPRESSURELEVEL] = CRGB::Red;
    } else if (application_state.rocketPressurePsi < 75) {
        leds[LED_FOR_ROCKETPRESSURELEVEL] = CRGB::Yellow;
    } else {
        leds[LED_FOR_ROCKETPRESSURELEVEL] = CRGB::Green;
    }

    // Solenoid
    if (application_state.isSolendoidOpen) {
        leds[LED_FOR_SOLENOID] = CRGB::Green;
    } else {
        leds[LED_FOR_SOLENOID] = CRGB::Red;
    }

    //Res pressure
    if (application_state.resevoirPressurePsi < 50) {
        leds[LED_FOR_RESPRESSURELEVEL] = CRGB::Red;
    } else if (application_state.resevoirPressurePsi < 75) {
        leds[LED_FOR_RESPRESSURELEVEL] = CRGB::Yellow;
    } else {
        leds[LED_FOR_RESPRESSURELEVEL] = CRGB::Green;
    }

    if (isButtonThrobbing && throbbingButton.isDirty) {

        throbbingButton.isDirty = false;
    }

    FastLED.show();
}

void ControllerDisplay::UpdateSound() {
    if (! isNewAppState) return;
}

// ANIMATIONS //////////////////////////

void ControllerDisplay::ThrobbingButton(bool isTurnOn) {

    if (isButtonThrobbing == isTurnOn)  return;

    switch (isTurnOn) {
        case false:
            taskManager.cancelTask(throbbingButton.taskId);
            throbbingButton.taskId = 0;
            isButtonThrobbing = false;
            break;

        case true:
            throbbingButton.taskId = taskManager.schedule(repeatMillis(1000), ProcessThrobbingButton);
            isButtonThrobbing = true;
            break;
    }
}

void ControllerDisplay::SetEspNowActive(bool cond) {
    Log.infoln("SetEspNowActive called");
    isEspNowActive = cond;

};

void ProcessThrobbingButton() {
    switch (throbbingButton.isButtonGoingUp) {
        case true:
            if (throbbingButton.currentBrightnessLevel + throbbingButton.increaseChangeIncrement > throbbingButton.maximumBrightnessLevel) {
                throbbingButton.isButtonGoingUp = false;
            } else {
                throbbingButton.currentBrightnessLevel += throbbingButton.increaseChangeIncrement;
                throbbingButton.isDirty = true;
            }
        break;
        case false:
            if (throbbingButton.currentBrightnessLevel - throbbingButton.decreaseChangeIncrement < throbbingButton.minimumBrightnessLevel) {
                throbbingButton.isButtonGoingUp = true;
            } else {
                throbbingButton.currentBrightnessLevel -= throbbingButton.decreaseChangeIncrement;
                throbbingButton.isDirty = true;
            }
        break;
    }
}




// SETUP ///////////////////////////////

void ControllerDisplay::SetupBuzzer() {

   /* int melody[] = {
        NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
      };

    int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

    for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZER_PIN, melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZER_PIN);
    }
    */

}

void ControllerDisplay::SetupDisplays() {

    if(!oledDisplay1.begin(SSD1306_SWITCHCAPVCC, SCREEN1_OLED_ADDRESS)) {
        Log.errorln("SSD1306 allocation failed");
        for(;;); // Don't proceed, loop forever @TODO: Fix this
    }

    if(!oledDisplay2.begin(SSD1306_SWITCHCAPVCC, SCREEN2_OLED_ADDRESS)) {
        Log.errorln("SSD1306 allocation failed 2");
        for(;;); // Don't proceed, loop forever @TODO: Fix this
    }

    application_state.resevoirPressureVoltage = 0;
    application_state.resevoirPressurePsi = 0;
    application_state.rocketPressurePsi = 0;
    application_state.rocketPressureVoltage = 0;
    application_state.isDirty = true;
    UpdateDisplays();

    Log.noticeln("Setup Display completed");
}

void ControllerDisplay::SetupLights() {
    Log.infoln("Starting LED Intialization");
    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, LED_TOTAL_LEDS);
    for(int i = 0; i < LED_TOTAL_LEDS; i++) {
        FastLED.clear();
        leds[i] = CRGB::Red;
        if (i > 2) {
            leds[i-1] = CRGB::Green;
        }

        if (i > 1) {
            leds[i-2] = CRGB::Blue;
        }

        FastLED.show();
        delay(50);
    }
//    delay(1000);
    FastLED.clear();
    FastLED.show();

    Log.noticeln("setupLights complete");
}








