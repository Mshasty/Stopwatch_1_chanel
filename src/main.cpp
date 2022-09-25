#include <Arduino.h>
#include <Wire.h>
//#include <SPI.h>

//#define DEBUG_SERIAL


//#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
//#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// SPI hardware interface
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary pins
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Text parameters
//#define CHAR_SPACING  1 // pixels between characters

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  8
char message[BUF_SIZE] = " CTAPT ";

#include "max7219.h"        // Драйвер дисплея
//#include "LED_output.h"
#include "timing.h"
#include "my_button.h"

void update(uint16_t interval);

void setup() {
  //mx.begin();
  //mx.setFont(ExtASCII);
  button1.begin();
  button2.begin();
  Serial.begin(57600);
  Serial.print(F("\nStart timer\nMaximum time: 9:59.9\n\n"));
  max7219_setup();
  //Serial.print(F("MAX7219 Setup succesful\n"));
  //max_display("Секундомер до 10 минут", true, 0);
  P.displayText("Секундомер до 10 минут", PA_LEFT, 40, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT); 
  Serial.print(F("Buffer ready\n"));
  //utf8rus(max_screen[max_current_screen]).toCharArray(max_buf, 512);
  //P.displayScroll(max_buf, PA_LEFT, PA_SCROLL_LEFT, 70 - max_speed);
  //Serial.print(F("Display handle function\n"));
  
  Serial.begin(57600);
  Serial.print(F("\nStart timer\nMaximum time: 9:59.9\n\n"));
  //oldtime = millis();
  //printText(0, MAX_DEVICES-1, message);
  //delay(2000);
}

void loop() {
  max_handle();
  update(100);
  if (pressed) {
  if (millis() > keytime)
    pressed = false;
  } else {
    switch( button1.Loop() ){
      case SB_CLICK:
        if (stopped) {
          oldtime = millis();
          stopped = false;
          started = true;
          paused = false;
          myTime = 0;
          Serial.println(F("Start counting"));
        } else if (paused && started) {
          //stopped = true;
          started = false;
          ;paused = false;
          Serial.println(F("Stop counting"));
          keytime = millis() + 2000; // игнор кнопок 2 секунды
          pressed = true;
        } else
          Serial.println(F("Pressed button 1"));
        break;
      case SB_LONG_CLICK:
        if (paused || stopped || !started) {
          //message[BUF_SIZE] = " CTAPT ";
          max_display(" CTAPT \0", false, 0);
          max_handle(); // поддержка работы светодиодной матрицы
          //printText(0, MAX_DEVICES-1, " CTAPT \0");
          started = false;
          paused = false;
          stopped = true;
          keytime = millis() + 2500; // игнор кнопок 2.5 секунды
          pressed = true;
          Serial.println(F("Reset all counter"));
        } else 
          Serial.println(F("Holded button 1"));
        break;
    }
    switch( button2.Loop() ){
      case SB_CLICK:
        if (started) {
          if (paused) {
            paused = false;
            Serial.println(F("Account continued"));
          } else {
            paused = true;
            Serial.println(F("Account suspended"));
          }
          keytime = millis() + 1000; // игнор кнопок 1 секунду
          pressed = true;
        } else
          Serial.println(F("Press button 2"));
        break;
      case SB_LONG_CLICK:
        if (started) {
          started = false;
          Serial.println(F("Account stopped"));
        } else
          Serial.println(F("Holded button 2"));
        break;
    }
  }
}

void update(uint16_t interval) {
  static unsigned long timer;
  static unsigned int interval_ = interval;

  if(timer + interval_ > millis() || max_animate) // Если интервал еще не истек, или идет нанимация
    return;
  timer = millis();
  if (started) {
    NextTime();
    //max_display(message, false, 0);
    P.displayText(message, PA_CENTER, 0, 100, PA_NO_EFFECT, PA_NO_EFFECT);
  }  
}
