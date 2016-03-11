#include <SPI.h>
#include <MySensor.h>  

#define DIGITAL_INPUT_SENSOR 3   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define INTERRUPT DIGITAL_INPUT_SENSOR-2 // Usually the interrupt = pin -2 (on uno/nano anyway)
#define CHILD_ID_MOTION 1   // Id of the sensor child

#define CHILD_ID_LIGHT 0
#define LIGHT_SENSOR_ANALOG_PIN 0

unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)

MySensor gw;
MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
int lastLightLevel;

// Initialize motion message
MyMessage msgmotion(CHILD_ID_MOTION,  V_TRIPPED);


void setup()  
{ 
  gw.begin();

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("MotionSensor", "40.3");

  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);
  
  pinMode(DIGITAL_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_MOTION, S_MOTION);
}

void loop()      
{     
  int lightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23; 
  Serial.print("Lightlevel: ");
  Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
      // gw.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
  gw.send(msg.set(lightLevel));
  
  // Read digital motion value
  boolean tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH; 
        
  Serial.println(tripped);
  gw.send(msgmotion.set(tripped?"1":"0"));  // Send tripped value to gw 
 
  // Sleep until interrupt comes in on motion sensor. Send update every two minute. 
  gw.sleep(INTERRUPT,CHANGE, SLEEP_TIME);
  
  
  // gw.sleep(SLEEP_TIME);
}



