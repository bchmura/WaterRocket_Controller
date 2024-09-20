#ifndef CONFIG_HPP
#define CONFIG_HPP

constexpr unsigned int I2C_1_SDA = 21;
constexpr unsigned int I2C_1_SDL = 22;

constexpr unsigned int SCREEN1_OLED_WIDTH = 128;
constexpr unsigned int SCREEN1_OLED_HEIGHT = 64;
constexpr int SCREEN1_OLED_RESET_PIN = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
constexpr unsigned int SCREEN1_OLED_ADDRESS  = 0x3D;

constexpr unsigned int SCREEN2_OLED_WIDTH = 128;
constexpr unsigned int SCREEN2_OLED_HEIGHT = 64;
constexpr int SCREEN2_OLED_RESET_PIN = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
constexpr unsigned int SCREEN2_OLED_ADDRESS  = 0x3C;

constexpr unsigned int LED_BRIGHTNESS = 125;


constexpr unsigned int BUZZER_PIN = 4;

constexpr unsigned int BUTTON1_PIN = 100;

constexpr unsigned int BUTTON2_PIN = 200;

constexpr unsigned int LED_TOTAL_LEDS = 31;
constexpr unsigned int LED_DATA_PIN = 27;

constexpr unsigned int LED_FOR_NETWORK = 10;
//constexpr unsigned int LED_HIDDEN_0 = 0;
constexpr unsigned int LED_FOR_ESPNOWACTIVE = 1;
constexpr unsigned int LED_FOR_UPDATE = 2;
constexpr unsigned int LED_FOR_LAUNCHERVERTICAL = 3;
constexpr unsigned int LED_FOR_ROCKETPRESSURELEVEL = 4;
constexpr unsigned int LED_FOR_SOLENOID = 5;
constexpr unsigned int LED_FOR_RESPRESSURELEVEL = 6;
constexpr unsigned int LED_FOR_UNUSED7 = 7;
constexpr unsigned int LED_FOR_UNUSED8 = 8;
constexpr unsigned int LED_FOR_UNUSED9 = 9;
constexpr unsigned int LED_FOR_UNUSED10 = 10;
constexpr unsigned int LED_FOR_UNUSED11 = 11; // last visable
//constexpr unsigned int LED_FOR_UNUSED12 = 12;
//constexpr unsigned int LED_FOR_UNUSED = 13;
constexpr unsigned int LED_LOADING_ANIM_START = 14;
//constexpr unsigned int LED_FOR_UNUSED = 15;
constexpr unsigned int LED_LOADING_ANIM_STOP = 16;
//constexpr unsigned int LED_FOR_UNUSED = 17; // not visable
//constexpr unsigned int LED_FOR_UNUSED = 18; // not visable
constexpr unsigned int LED_FOR_UNUSED = 19;
constexpr unsigned int LED_FOR_UNUSED20 = 20;
constexpr unsigned int LED_FOR_UNUSED21 = 21;
constexpr unsigned int LED_FOR_UNUSED22 = 22;
constexpr unsigned int LED_FOR_UNUSED23 = 23; // not visable
constexpr unsigned int LED_REDBUTTON_START = 24;
constexpr unsigned int LED_REDBUTTON_STOP = 31;

#endif //CONFIG_HPP
