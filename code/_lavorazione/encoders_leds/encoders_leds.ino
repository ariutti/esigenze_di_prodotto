// SERIAL STUFF ////////////////////////////////////////////////////////
bool bSendSerial = false;

//



// /////////////
// STATE MACHINE
// /////////////

enum {
  STANDBY=0,
  SEARCH, 
  SELECTED
} status = STANDBY;

long prevTime, waitTime = 5000;


// //////////////
// ANIMATOR STUFF
// //////////////

#include "Animator_Sine.h"
int pos = 0;
Animator_Sine animSine;

#include "Animator_AR.h"
Animator_AR animAR;

float y = 0.0;


// ENCODER STUFF /////////////////////////////////////////////////////// 
// Encoder code is inspired by the work of eran.io
// http://eran.io/rotary-encoder-based-cooking-timer/
// Here's code: https://github.com/pavius/rotary-encoder-timer

#include "eran_encoder.h"
long counter = 0;

EranEncoder encoder;
int eValue, ePrevValue;

void cwStep(void){
  if( bSendSerial) Serial.write( '0' );
  counter ++;
  
  status = SEARCH;
  prevTime = millis();
}

void ccwStep(void){
  if( bSendSerial) Serial.write( '1' );
  counter --;
  
  status = SEARCH;
  prevTime = millis();
}



// BUTTON STUFF ////////////////////////////////////////////////////////
// (ButtonDebounce Library is written and maintained 
// by Maykon L. Capellari <maykonluiscapellari@gmail.com>
// https://github.com/maykon/ButtonDebounce

#include "ButtonDebounce.h"

ButtonDebounce push(12, 50); //ButtonDebounce push(6, 50);


void pushChanged(int state){
  if( !state )
  {
    if( bSendSerial) Serial.write('c');
    //if(status == STANDBY || status == SEARCH)
    //{
      Serial.println("cliccato");
      status = SELECTED;
      animAR.trigger();
      prevTime = millis();
    //}
    //else if(status == SELECTED )
    //{
      // do nothing
      //prevTime = millis();
    //}
  }
}

// /////////
// LED STUFF
// /////////

// this is the order of the colors GREEN, RED, BLUE
#include "Adafruit_DotStar.h"
#include <SPI.h>         // see Adafruit DotStar library example to better understand

#define NLEDS 18 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    8//4
#define CLOCKPIN   9//5
Adafruit_DotStar strip = Adafruit_DotStar(NLEDS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
int intensity = 255;
int color = 0;


// ///////
// UTILITY
// ///////
int i, j = 0;
int blue = 0;

// SETUP ///////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  encoder.init(10,11); //encoder.init(2,3);
  encoder.setCallbackCW(cwStep);
  encoder.setCallbackCCW(ccwStep);

  push.setCallback(pushChanged);

  animSine.init(0.5, 0.0);
  animAR.init(100, 500);


  // LED STUFF *********************************************************************/
  strip.begin(); // Initialize pins for output
  // set every pixel to sleep
  for(i=0; i<NLEDS; i++) 
  {
    strip.setPixelColor(i, 0x00, 0x00, 0x00);
  }
  strip.show();  // Turn all LEDs off ASAP 
}

// LOOP ////////////////////////////////////////////////////////////////

void loop() 
{
  getSerialData();
  
  encoder.update();
  push.update();

  if( status == STANDBY )
  {
    animSine.update();
    y = animSine.getY();
  } 
  else if( status == SEARCH )
  {
    y = 0;
    if( (millis()-prevTime) < waitTime )
    {
      //Serial.println("case SEARCH: time < waitTime");
      //Serial.println(status);
      counter = counter % NLEDS;
      if(counter < 0)
        counter = NLEDS-1;
      //Serial.println(counter);
      for(i=0; i<NLEDS; i++)
      {
        if(i == counter)
        {
          //Serial.print("accendi questo led - ");
          //Serial.println(i);
          strip.setPixelColor(i, intensity, intensity, intensity);
        }
        else
          strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
    else
    {
      //Serial.println("case SEARCH: time > waitTime");
      status = STANDBY;
      prevTime = millis();
    }
  }
  else if( status == SELECTED )
  {
    animAR.update();
    y = animAR.getY();
    if( (millis()-prevTime) < waitTime )
    {
      //Serial.println("case SELECTED: time < waitTime");
    }
    else
    {
      //Serial.println("case SELECTED: time > waitTime");
      status = STANDBY;
      prevTime = millis();
    }
  }

  //Serial.println(millis() - prevTime);
  //Serial.print(y); Serial.print(" - ");
  //printStates();

  // LED STUFF *********************************************************************/
  // The logic below is created in order to properly light-up
  // leds around the centroid according to the 
  // squared raised-cosine lookup table.

  if( status == STANDBY || status == SELECTED )
  {
    for(i=0; i<NLEDS; i++)
    {
      color = y*y*255;
      strip.setPixelColor(i, color, color, color);
    }
  } 
  else
  {
    // we are in SEARCH state
    counter = counter % NLEDS;
    if(counter < 0 ) counter = NLEDS-1;
    for(i=0; i<NLEDS; i++)
    {
      if( i == counter)
      {
        int j = i-1;
        if(j<0) j = NLEDS -1;
        strip.setPixelColor(i, intensity, intensity, intensity);
        strip.setPixelColor(j, intensity, intensity, intensity);
      }
      else
      {
        strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
  }
  
  strip.show();
  
  delay(5); 
}

// SERIAL UTILITY FUNC /////////////////////////////////////////////////
void getSerialData()
{
  if(Serial.available()) {
    char user_input = Serial.read(); // Read user input and trigger appropriate function
      
    if (user_input =='1')  {
       bSendSerial = true;
    }
    else if(user_input =='0')  {
      bSendSerial = false;
    }
  }
}

void printStates() 
{
  if( status == STANDBY )
  {
    Serial.println("STANDBY");
  }
  else if( status == SEARCH )
  {
    Serial.println("SEARCH");
  }
  else if ( status == SELECTED )
  {
    Serial.println("SELECTED");
  } 
  else
  {
    Serial.println("??");
  }
}

