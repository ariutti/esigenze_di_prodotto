/* 
 MODULE 1 CONNECTIONS:
 * first LED strip VCC to external 5V power supply
 * first LED strip GND to external Arduino GND;
 * first LED strip DATA to Arduino digital pin 2 (through a 470 Ohm resistor);
 * first LED strip CLOCK to Arduino digital pin 3 (through a 470 Ohm resistor);
 * first encoder CHA pin to Arduino digital pin 4
 * first encoder CHB pin to Arduino digital pin 5
 * first encoder PUSH pin to Arduino digital pin 6


 MODULE 2 CONNECTIONS:
 * second LED strip VCC to external 5V power supply
 * second LED strip GND to external Arduino GND;
 * second LED strip DATA to Arduino digital pin 8 (through a 470 Ohm resistor);
 * second LED strip CLOCK to Arduino digital pin 9 (through a 470 Ohm resistor);
 * first encoder CHA pin to Arduino digital pin 10;
 * first encoder CHB pin to Arduino digital pin 11;
 * first encoder PUSH pin to Arduino digital pin 12;

 NOTE: place a 6,3V 1000 microF capacitor between LEDs GND and VCC rails;
 Place one 10K pullup resistor for each encoder pin;

 SERIAL COMMUNICATION PROTOCOL

 <encoder SX (+1/-1) > | < push SX> | <encoder DX (+1/-1) > | < push DX> | <a capo>
*/

// //////////////
// PIN DEFINITION
// //////////////

// module 1
#define M1_LEDSTRIP_DATA_PIN      2
#define M1_LEDSTRIP_CLOCK_PIN     3
#define M1_ENC_CHA_PIN            5
#define M1_ENC_CHB_PIN            4
#define M1_ENC_PUSH_PIN           6

#define M2_LEDSTRIP_DATA_PIN      8
#define M2_LEDSTRIP_CLOCK_PIN     9
#define M2_ENC_CHA_PIN           11
#define M2_ENC_CHB_PIN           10
#define M2_ENC_PUSH_PIN          12


// /////////////
// STATE MACHINE
// /////////////

enum m_statuses{
  STANDBY=0,
  SEARCH, 
  SELECTED
} m1_status=STANDBY, m2_status=STANDBY;

long m1_prevTime, m2_prevTime;
long m1_waitTime = 5000;
long m2_waitTime = 5000;



// //////////////
// ANIMATOR STUFF
// //////////////

#include "Animator_Sine.h"
Animator_Sine m1_animSine;
Animator_Sine m2_animSine;

#include "Animator_AR.h"
Animator_AR m1_animAR;
Animator_AR m2_animAR;

float m1_y = 0.0;
float m2_y = 0.0;

// /////////////
// ENCODER STUFF 
// /////////////

// Encoder code is inspired by the work of eran.io
// http://eran.io/rotary-encoder-based-cooking-timer/
// Here's code: https://github.com/pavius/rotary-encoder-timer

#include "eran_encoder.h"
long m1_counter = 0;
long m2_counter = 0;

EranEncoder m1_encoder; 
EranEncoder m2_encoder;



void m1_cwStep(void)
{
  m1_counter --;
  
  m1_status = SEARCH;
  m1_prevTime = millis();
  //Serial.println("1,+1;");
  Serial.println("-1|0|0|0");
}

void m1_ccwStep(void)
{
  m1_counter ++;
  
  m1_status = SEARCH;
  m1_prevTime = millis();
  //Serial.println("1,-1;");
  Serial.println("+1|0|0|0");
}

void m2_cwStep(void)
{
  m2_counter --;
  
  m2_status = SEARCH;
  m2_prevTime = millis();
  //Serial.println("2,+1;");
  Serial.println("0|0|-1|0");
}

void m2_ccwStep(void)
{
  m2_counter ++;
  
  m2_status = SEARCH;
  m2_prevTime = millis();
  //Serial.println("2,-1;");
  Serial.println("0|0|+1|0");
}


// ////////////
// BUTTON STUFF
// ////////////

#include "ButtonDebounce.h"

// (ButtonDebounce Library is written and maintained 
// by Maykon L. Capellari <maykonluiscapellari@gmail.com>
// https://github.com/maykon/ButtonDebounce

ButtonDebounce m1_push(M1_ENC_PUSH_PIN, 50);
ButtonDebounce m2_push(M2_ENC_PUSH_PIN, 50);

void m1_pushChanged(int state){
  if( !state )
  {
    m1_status = SELECTED;
    m1_animAR.trigger();
    m1_prevTime = millis();
    //Serial.println("1,P;");
    Serial.println("0|1|0|0");
  }
}

void m2_pushChanged(int state){
  if( !state )
  {
    m2_status = SELECTED;
    m2_animAR.trigger();
    m2_prevTime = millis();
    //Serial.println("2,P;");
    Serial.println("0|0|0|1");
  }
}


// /////////
// LED STUFF
// /////////

// this is the order of the colors GREEN, RED, BLUE
#include "Adafruit_DotStar.h"
#include <SPI.h> // see Adafruit DotStar library example to better understand

#define NLEDS 21 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
Adafruit_DotStar m1_strip = Adafruit_DotStar(NLEDS, M1_LEDSTRIP_DATA_PIN, M1_LEDSTRIP_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar m2_strip = Adafruit_DotStar(NLEDS, M2_LEDSTRIP_DATA_PIN, M2_LEDSTRIP_CLOCK_PIN, DOTSTAR_BRG);

int m1_intensity = 255; 
int m2_intensity = 255;
int m1_color = 0;
int m2_color = 0;
// these values are used during the "breathing" phase
int m1_max_intensity = 60; 
int m2_max_intensity = 60; 


// ///////
// UTILITY
// ///////
int i, j = 0;

// SETUP ///////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);

  // MODULE 1
  
  m1_encoder.init(M1_ENC_CHA_PIN, M1_ENC_CHB_PIN);
  m1_encoder.setCallbackCW(m1_cwStep);
  m1_encoder.setCallbackCCW(m1_ccwStep);
  m1_push.setCallback(m1_pushChanged);
  m1_animSine.init(0.5, 0.0);
  m1_animAR.init(100, 500);
  // LED STUFF 
  m1_strip.begin(); // Initialize pins for output
  // set every pixel to sleep
  for(i=0; i<NLEDS; i++) 
  {
    m1_strip.setPixelColor(i, 0x00, 0x00, 0x00);
  }
  m1_strip.show();  // Turn all LEDs off ASAP 

  // MODULE 2

  m2_encoder.init(M2_ENC_CHA_PIN, M2_ENC_CHB_PIN);
  m2_encoder.setCallbackCW(m2_cwStep);
  m2_encoder.setCallbackCCW(m2_ccwStep);
  m2_push.setCallback(m2_pushChanged);
  m2_animSine.init(0.5, 0.0);
  m2_animAR.init(100, 500);
  // LED STUFF 
  m2_strip.begin(); // Initialize pins for output
  // set every pixel to sleep
  for(i=0; i<NLEDS; i++) 
  {
    m2_strip.setPixelColor(i, 0x00, 0x00, 0x00);
  }
  m2_strip.show();  // Turn all LEDs off ASAP 
}

// LOOP ////////////////////////////////////////////////////////////////

void loop() 
{

  // MODULE 1
  
  m1_encoder.update();
  m1_push.update();

  if( m1_status == STANDBY )
  {
    m1_animSine.update();
    m1_y = m1_animSine.getY();
  } 
  else if( m1_status == SEARCH )
  {
    m1_y = 0;
    if( (millis()-m1_prevTime) < m1_waitTime )
    {
      m1_counter = m1_counter % NLEDS;
      if(m1_counter < 0) m1_counter = NLEDS-1;
      for(i=0; i<NLEDS; i++)
      {
        if(i == m1_counter)
        {
          m1_strip.setPixelColor(i, m1_intensity, m1_intensity, m1_intensity);
        }
        else
          m1_strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
    else
    {
      m1_status = STANDBY;
      m1_prevTime = millis();
    }
  }
  else if( m1_status == SELECTED )
  {
    m1_animAR.update();
    m1_y = m1_animAR.getY();
    if( (millis()-m1_prevTime) < m1_waitTime )
    {
      // do nothing
    }
    else
    {
      m1_status = STANDBY;
      m1_prevTime = millis();
    }
  }

  // LED STUFF

  if( m1_status == STANDBY || m1_status == SELECTED )
  {
    for(i=0; i<NLEDS; i++)
    {
      m1_color = m1_y*m1_y*m1_max_intensity;
      m1_strip.setPixelColor(i, m1_color, m1_color, m1_color);
    }
  } 
  else
  {
    // we are in SEARCH state
    m1_counter = m1_counter % NLEDS;
    if(m1_counter < 0 ) m1_counter = NLEDS-1;
    for(i=0; i<NLEDS; i++)
    {
      if( i == m1_counter)
      {
        int j = i-1;
        if(j<0) j = NLEDS -1;
        m1_strip.setPixelColor(i, m1_intensity, m1_intensity, m1_intensity);
        m1_strip.setPixelColor(j, m1_intensity, m1_intensity, m1_intensity);
      }
      else
      {
        m1_strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
  }
  
  m1_strip.show();


  // MODULE 2

  m2_encoder.update();
  m2_push.update();

  if( m2_status == STANDBY )
  {
    m2_animSine.update();
    m2_y = m2_animSine.getY();
  } 
  else if( m2_status == SEARCH )
  {
    m2_y = 0;
    if( (millis()-m2_prevTime) < m2_waitTime )
    {
      m2_counter = m2_counter % NLEDS;
      if(m2_counter < 0) m2_counter = NLEDS-1;
      for(i=0; i<NLEDS; i++)
      {
        if(i == m2_counter)
        {
          m2_strip.setPixelColor(i, m2_intensity, m2_intensity, m2_intensity);
        }
        else
          m2_strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
    else
    {
      m2_status = STANDBY;
      m2_prevTime = millis();
    }
  }
  else if( m2_status == SELECTED )
  {
    m2_animAR.update();
    m2_y = m2_animAR.getY();
    if( (millis()-m2_prevTime) < m2_waitTime )
    {
      // do nothing
    }
    else
    {
      m2_status = STANDBY;
      m2_prevTime = millis();
    }
  }

  if( m2_status == STANDBY || m2_status == SELECTED )
  {
    for(i=0; i<NLEDS; i++)
    {
      m2_color = m2_y*m2_y*m2_max_intensity;
      m2_strip.setPixelColor(i, m2_color, m2_color, m2_color);
    }
  } 
  else
  {
    // we are in SEARCH state
    m2_counter = m2_counter % NLEDS;
    if(m2_counter < 0 ) m2_counter = NLEDS-1;
    for(i=0; i<NLEDS; i++)
    {
      if( i == m2_counter)
      {
        int j = i-1;
        if(j<0) j = NLEDS -1;
        m2_strip.setPixelColor(i, m2_intensity, m2_intensity, m2_intensity);
        m2_strip.setPixelColor(j, m2_intensity, m2_intensity, m2_intensity);
      }
      else
      {
        m2_strip.setPixelColor(i, 0x00, 0x00, 0x00);
      }
    }
  }
  
  m2_strip.show();

  //m1PrintStates();
  //m2PrintStates();
  
  delay(1); 
}

void m1PrintStates() 
{
  if( m1_status == STANDBY )   {
    Serial.println("STANDBY");
  }
  else if( m1_status == SEARCH )   {
    Serial.println("SEARCH");
  }
  else if ( m1_status == SELECTED )   {
    Serial.println("SELECTED");
  } 
  else  {
    Serial.println("??");
  }
}

void m2PrintStates() 
{
  if( m2_status == STANDBY ) {
    Serial.println("STANDBY");
  }
  else if( m2_status == SEARCH ) {
    Serial.println("SEARCH");
  }
  else if ( m2_status == SELECTED ) {
    Serial.println("SELECTED");
  } 
  else {
    Serial.println("??");
  }
}

