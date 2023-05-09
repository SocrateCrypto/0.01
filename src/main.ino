
#include "display.h"
#include "Tick1000.h"
#include "mouse_interrumpt.h"




void setup()
{
   setup_display();
  pinMode(25,OUTPUT);
    mouse_setup();
    periodForTickLoop(100000);
  
   
}

void loop()
{
   mouse_loop(); 
   loop_display();
  
    
   
}



void setup1() {
    // setup_display();
    
    mouse_setup1();
    
    
}

void loop1()
{
    // loop_display();
    
    mouse_loop1();
     digitalWrite(25, HIGH);
    delay(100);
    digitalWrite(25, LOW);
    delay(100);
    
}


void tick_loop()
{
    // digitalWrite(25,!digitalRead(25));


}
