/*
  AnotherButton - non-standart control WLED with one another :) button
    - Click (togle On/Off)
    - Double Click (Next preset mode)
    - Triple Click (togle max/min brightness)
    - Long Press (gradually increase brightness)
    - Click + Long Press (fade out brightness)
  Used library: GyverButton
  User setup - see setup section
*/
#include <Arduino.h>
#include "GyverButton.h"

#include <cmath>

//////////////////////////////// Setup section /////////////////////////////////
// Another Button there: PIN --- Button --- +5V or +3.3V
#if defined(ARDUINO_ARCH_ESP32)
#define AB_BUTTON_PIN 17				
#elif defined(ESP8266)
#define AB_BUTTON_PIN 14				
#else
#define AB_BUTTON_PIN 3				
#endif
// If you want debug print in serial, uncoment next line and fun serial monitor :)
#define DEBUG_PRINT
// My sensor button connected to "+". If it connected to ground, then change to HIGHT_PULL
GButton another_button(AB_BUTTON_PIN, LOW_PULL, NORM_OPEN); 
// GButton another_button(AB_BUTTON_PIN, HIGHT_PULL, NORM_OPEN); 
#define AB_STEPS 8          // gradations for dimming
// Next setting individual. Можете не менять их, если вам комфортно при таких установках.
#define AB_DEBOUNCE 50      // настройка антидребезга (по умолчанию 80 мс)
#define AB_TIMEOUT 300      // настройка таймаута на удержание (по умолчанию 500 мс)
#define AB_CLICKTIMEOUT 300 // настройка таймаута между кликами (по умолчанию 300 мс)
#define AB_STEPTIMEOUT 300  // автоповтор при Hold
//////////////////////////////// Setup section /////////////////////////////////

#ifdef DEBUG_PRINT
int map_level(int level);
void print_level(int level);
void print_state(const char * s);
#endif

float step = 256. / AB_STEPS;
int level = AB_STEPS / 2;    // Can changed from 1 to AB_STEPS

boolean light_state = false;
boolean light_max = false;
int light_mode = 1;
int light_modes = 5;

void setup() {
  another_button.setDebounce(AB_DEBOUNCE);        
  another_button.setTimeout(AB_TIMEOUT);        
  another_button.setClickTimeout(AB_CLICKTIMEOUT); 
  another_button.setStepTimeout(AB_STEPTIMEOUT);    
  
#ifdef DEBUG_PRINT
  Serial.begin(9600);
  Serial.println("\nAhother Button (GyverButton) test...");
  Serial.printf("Pin of button = %d, gradations for dimming = %d", AB_BUTTON_PIN, AB_STEPS);
  Serial.println("\nPress button: click, duble click, triple click, hold, click+hold...");
    print_state(" ");
#endif    
}

void loop() {
  another_button.tick();

  if (another_button.isSingle()) {    // Togle Light
    light_state = !light_state;
#ifdef DEBUG_PRINT
    print_state((light_state ? "ON" : "OFF"));
#endif    
  } 
  if (another_button.isDouble()) {    // Next mode
    light_mode++;
    if (light_mode > light_modes) light_mode = 1;
#ifdef DEBUG_PRINT
    char ws[4];
    sprintf(ws, "%4d", light_mode);
    print_state(ws);
#endif    
  } 
  if (another_button.isTriple()) {    // Dimmr Min/Max Togle
    light_max = !light_max;
    level = level < AB_STEPS / 2 ? AB_STEPS : 1;  // if level < 50% ==> Max level and vice versa
#ifdef DEBUG_PRINT
    print_state((light_max ? "MAX" : "MIN"));
#endif    
  } 
  if (another_button.isStep() && level < AB_STEPS) {       // если кнопка была удержана (это для инкремента)
    level++;                                            // увеличивать/уменьшать переменную level с шагом и интервалом
#ifdef DEBUG_PRINT
    print_state("UP");
#endif    
  }
  if (another_button.isStep(1) && level > 1) {                                
    level--;                                            
#ifdef DEBUG_PRINT
    print_state("DOWN");
#endif    
  }
}

#ifdef DEBUG_PRINT
void print_state(const char * action) {
  Serial.printf("\n%4s, level %2d ", action, level);
  print_level(level);
  Serial.printf(" value %3d", (int)floor(level * step + 0.5) - 1);   
}

void print_level(int level) {
  for (size_t i = 0; i < AB_STEPS; i++) {
    Serial.print( i+1 <= level ? "=" : ".");
  }
}

int map_level(int level) {
  return (int)floor(level * step + 0.5) - 1;
}
#endif    
