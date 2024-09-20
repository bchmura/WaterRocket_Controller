//
// Created by bill on 9/10/2024.
//

#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <Wire.h>
#include <ArduinoLog.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <chrono>
#include <config.hpp>
#include "ApplicationStatus.hpp"

void ProcessThrobbingButton();

typedef struct {
    bool isButtonGoingUp = true;
    bool isDirty = false;
    unsigned int currentBrightnessLevel = 0;
    unsigned int minimumBrightnessLevel = 0;
    unsigned int maximumBrightnessLevel = 0;
    unsigned int increaseChangeIncrement = 10;
    unsigned int decreaseChangeIncrement = 10;
    unsigned int taskId = 0;
} throbbingButtonState;


class ControllerDisplay {
public:
    ControllerDisplay();
    void UpdateAppStatus(ApplicationState newAppState);
    void RunLoop();
    void Setup();
    void SetupLights();
    void SetupDisplays();
    void SetupBuzzer();
    void UpdateLights();
    void UpdateDisplays();
    void UpdateSound();
    void ThrobbingButton(bool isTurnOn);
    void SetEspNowActive(bool cond);

    bool isEspNowActive = false;
    std::chrono::system_clock::time_point lastAppUpdateTime;

private:

};


#endif //DISPLAY_HPP
