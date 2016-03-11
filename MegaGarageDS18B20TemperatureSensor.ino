// Example sketch showing how to send in OneWire DS18B20 temperature readings
#include <MySensor.h>  
#include <SPI.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht22.h>

// DHT22 sensor
dht DHT;
#define DHT22PIN 2
float lastTemp = 0;
float lastHum = 0;

// DS18 sensor
#define ONE_WIRE_BUS 			8 // Pin where Dallas sensor is connected 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress DS18 = { 0x28, 0xFF, 0xF8, 0x68, 0x15, 0x14, 0x00, 0xF4 }; // new
int lastTemperature = 0;

// Light sensor
int lastLightLevel = 0;
int lastLightLevel2 = 0;
#define LIGHT_SENSOR_ANALOG_PIN 0 //Outside LDR
#define LIGHT_GARAGE_SENSOR_ANALOG_PIN 1 //Garage LDR

// GW stuff
#define CHILD_ID_LIGHT 		0
#define CHILD_ID_TEMP_DS18  1
#define CHILD_ID_TEMP_DHT  	2
#define CHILD_ID_HUM_DHT  	3
#define CHILD_ID_LIGHT_Garage 		4

MySensor gw;

MyMessage tempds18Msg(CHILD_ID_TEMP_DS18,V_TEMP);
MyMessage lightMsg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
MyMessage tempdhtMsg(CHILD_ID_TEMP_DHT,V_TEMP);
MyMessage temphumMsg(CHILD_ID_HUM_DHT,V_HUM);
MyMessage lightgarageMsg(CHILD_ID_LIGHT_Garage, V_LIGHT_LEVEL);

unsigned long SLEEP_TIME = 50000; // Sleep time between reads (in milliseconds)
boolean metric = true;

void setup()  
{ 
  // Startup OneWire 
	sensors.begin();
	sensors.setResolution(DS18, 10);
  
  // Startup and initialize MySensors library. Set callback for incoming messages. 
  gw.begin(); 

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Garage Sensor", "22.3");
  gw.present(CHILD_ID_TEMP_DS18, S_TEMP);
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);
  gw.present(CHILD_ID_HUM_DHT, S_HUM);
  gw.present(CHILD_ID_TEMP_DHT, S_TEMP);
  gw.present(CHILD_ID_LIGHT_Garage, S_LIGHT_LEVEL);
  metric = gw.getConfig().isMetric;
}

void loop()     
{     
  // Process incoming messages (like config from server)
  gw.process(); 
  
  // Process DH22 sensor
  // To be added
	int chk = DHT.read22(DHT22PIN); 
	Serial.println();
	Serial.print("Read DHT22 sensor: ");
	switch (chk)
		{
    case DHTLIB_OK: 
		Serial.println("OK - continue processing"); 
		// float temperature = DHT.temperature;
		if (isnan(DHT.temperature)) {
			Serial.println("Failed reading temperature from DHT");
		} else if (DHT.temperature != lastTemp) {
			lastTemp = DHT.temperature;
			if (!metric) {
		//    temperature = DHT.toFahrenheit(temperature);
		}
		gw.send(tempdhtMsg.set(DHT.temperature, 1));
		Serial.print("Temperature: ");
		Serial.println(DHT.temperature);
		}
		//Serial.println(DHT.temperature);
		delay(1000);
	
//		float humidity = DHT.humidity;
		if (isnan(DHT.humidity)) {
			Serial.println("Failed reading humidity from DHT");
		} else if (DHT.humidity != lastHum) {
			lastHum = DHT.humidity;
			}
			gw.send(temphumMsg.set(DHT.humidity, 1));
			Serial.print("Humidity: ");
			Serial.println(DHT.humidity);
		
		//  Serial.println(DHT.humidity);
		  delay(1000);
		break;
		
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
		
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
		
    default: 
		Serial.println("Unknown error"); 
		break;
  }

  // Fetch temperatures from Dallas sensors
  sensors.requestTemperatures();
	delay (2000);
 
    // Fetch and round temperature to one decimal
    // float temperature = static_cast<float>(static_cast<int>((gw.getConfig().isMetric?sensors.getTempCByIndex(i):sensors.getTempFByIndex(i)) * 10.)) / 10.;
 
    // Only send data if temperature has changed and no error
    printTemperature(DS18);
	
	// Light level - outside
	int lightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23;    // Was 10.23, lager = gevoeliger
	Serial.print("Lightlevel outside= ");	
	Serial.println(lightLevel);
	if (lightLevel != lastLightLevel) {     
      lastLightLevel = lightLevel;
	}
	gw.send(lightMsg.set(lightLevel)); 
	// gw.sleep(SLEEP_TIME);

	// Light garage
	int lightLevel2 = (1023-analogRead(LIGHT_GARAGE_SENSOR_ANALOG_PIN))/10.23;    // Was 10.23
	Serial.print("Lightlevel garage= ");	
	Serial.println(lightLevel2);
	if (lightLevel2 != lastLightLevel2) {
      lastLightLevel2 = lightLevel2;
	}
  gw.send(lightgarageMsg.set(lightLevel2)); 
  gw.sleep(SLEEP_TIME);
}




// Read DS18x20 
void printTemperature(DeviceAddress deviceAddress)
{
  float temperature = sensors.getTempC(deviceAddress);
  if (temperature == -127.00) {
    Serial.println("Error getting DS18x20 temperature");
  } else {
		Serial.print("DS18x20 Temperature(oC): ");
		Serial.println(temperature);
		if (lastTemperature != temperature && temperature != -127.00) {
		  // Send in the new temperature
		  
		  lastTemperature=temperature;
		}
		gw.send(tempds18Msg.set(temperature,1));
		Serial.println("DS18x20 update sent");
	}
}
// End DS18x20
