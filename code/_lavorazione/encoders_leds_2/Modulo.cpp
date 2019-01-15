#include "Modulo.h"


void Modulo::cwStep(void){
  //if( bSendSerial) Serial.write( '0' );
  counter ++;
  
  status = SEARCH;
  prevTime = millis();
}

void Modulo::ccwStep(void){
  //if( bSendSerial) Serial.write( '1' );
  counter --;
  
  status = SEARCH;
  prevTime = millis();
}


void Modulo::pushChanged(int state){
  if( !state )
  {
    //if( bSendSerial) Serial.write('c');
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

void Modulo::init()
{
  push = new ButtonDebounce(12, 50); //ButtonDebounce push(6, 50);
  
  encoder.init(10,11); //encoder.init(2,3);
  encoder.setCallbackCW(cwStep);
  encoder.setCallbackCCW( ccwStep );

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

void Modulo::update()
{
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
}


