#pragma once

#include "wled.h"

//This is an empty v2 usermod template. Please see the file usermod_v2_example.h in the EXAMPLE_v2 usermod folder for documentation on the functions you can use!

#define TOUCHPIN1 14


class UsermodMultiButton : public Usermod {
  private:
    unsigned long lastChecked = 0;
    unsigned long lastTouch = 0;        //Timestamp of last Touch
    unsigned long lastRelease = 0;      //Timestamp of last Touch release
    boolean released = true;            //current Touch state (touched/released)
    uint16_t touchDuration = 0;         //duration of last touch
    boolean dim_dir = false;            //false for decreasing brightness, true for increasing brightness
    boolean touchReading = false;
    int set_bri = 0;

    bool checkButtonpress(){
         if(digitalRead(TOUCHPIN1) == LOW){
             return true;
         }
         else{
             return false;
         }
    }
  public:
    void setup() {
        pinMode(TOUCHPIN1, INPUT_PULLUP);
        lastChecked = millis();
    }

    void loop() {
        if (millis() - lastChecked >= 50) {                     //Check every 50ms if a touch occurs
        lastChecked = millis();
        touchReading = checkButtonpress();                      //Read touch pin 
        
        if(touchReading && released) {                          //Touch started
          released = false;
          lastTouch = millis();
          touchDuration = 0;
        }
        else if(!touchReading && !released) {                   //Touch released
          released = true;
          lastRelease = millis();
          touchDuration = lastRelease - lastTouch;               //Calculate duration
        }
        else if(touchReading && !released) {                    //Touch still pressed
          touchDuration = millis() - lastTouch;                 //Calculate current duration
        }
        
        //Serial.println(touchDuration);


        if(touchDuration >= 800 && !released) {              //dimm up and down when button is held down
            if(dim_dir){
                set_bri = bri + (touchDuration-800)*0.001;
                if(set_bri > 255)
                {
                  set_bri = 255;
                }
                Serial.print("dimming up! --> current brightness is: ");
                Serial.println(set_bri);
            }
            else{
                set_bri = bri - (touchDuration-800)*0.001;
                if(set_bri < 3)
                {
                  set_bri = 3;
                }
                Serial.print("dimming down! --> current brightness is: ");
                Serial.println(set_bri);
            }
            bri = set_bri;
            colorUpdated(2);  
        }
        else if(touchDuration >= 800 && released) 
        {
            dim_dir = !dim_dir;
            touchDuration = 0;                                    // change dimming direction for next button-long-press
        }
        else if(touchDuration >= 100 && released) {                   //Toggle power if button press is shorter than 800ms
          touchDuration = 0;                                     //Reset touch duration to avoid multiple actions on same touch
          toggleOnOff();
          colorUpdated(2);                                       //Refresh values
        }                                     //Refresh values

      }
    }
};
