// SERIAL STUFF ////////////////////////////////////////////////////////
bool bSendSerial = false;

//



// /////////////
// STATE MACHINE
// /////////////


// //////////////
// ANIMATOR STUFF
// //////////////




// ENCODER STUFF /////////////////////////////////////////////////////// 
// Encoder code is inspired by the work of eran.io
// http://eran.io/rotary-encoder-based-cooking-timer/
// Here's code: https://github.com/pavius/rotary-encoder-timer




// BUTTON STUFF ////////////////////////////////////////////////////////
// (ButtonDebounce Library is written and maintained 
// by Maykon L. Capellari <maykonluiscapellari@gmail.com>
// https://github.com/maykon/ButtonDebounce


// /////////
// LED STUFF
// /////////

// this is the order of the colors GREEN, RED, BLUE
#include "Adafruit_DotStar.h"
#include <SPI.h>         // see Adafruit DotStar library example to better understand




// ///////
// UTILITY
// ///////
int i, j = 0;

// SETUP ///////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  
}

// LOOP ////////////////////////////////////////////////////////////////

void loop() 
{
  getSerialData();
  
  

  modulo.update();
  
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

