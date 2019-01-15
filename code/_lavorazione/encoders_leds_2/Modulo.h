/*
  Modulo
  Nicola Ariutti
  2018/09/25
*/

#ifndef MODULO_H
#define MODULO_H

#include "Arduino.h"

#include "Animator_Sine.h"
#include "Animator_AR.h"
#include "eran_encoder.h"
#include "ButtonDebounce.h"

#include "Adafruit_DotStar.h"
#include <SPI.h>         // see Adafruit DotStar library example to better
#define NLEDS 18 // Number of LEDs in strip 


class Modulo
{
  private:

    // STATE MACHINE
    enum {
      STANDBY=0,
      SEARCH, 
      SELECTED
    } status = STANDBY;
    long prevTime, waitTime = 5000;

    // Animator stuff
    Animator_Sine animSine;
    Animator_AR animAR;
    float y = 0.0;

    // encoder stuff
    long counter = 0;
    EranEncoder encoder;
    int eValue, ePrevValue;

    // button
    ButtonDebounce *push;

    // Here's how to control the LEDs from any two pins:
    int DATAPIN  = 8;//4
    int CLOCKPIN = 9;//5
    Adafruit_DotStar strip = Adafruit_DotStar(NLEDS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
    int intensity = 255;
    int color = 0;

    
  public:
    Modulo() {};
    ~Modulo() {};

    void cwStep();
    void ccwStep();
    void pushChanged(int state);

    void init();
    void update();
};

#endif
