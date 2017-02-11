#include "application.h"
#include "beam.h"
#include "float2str.h"
#include "Adafruit_DHT/Adafruit_DHT.h"

// Define DHT22
#define DHTTYPE DHT22		// DHT 22 (AM2302)
#define DHTPIN 5     // what pin we're connected to

/* pin definitions for Beam */
#define RSTPIN 2        //use any digital pin
#define IRQPIN 9        //currently not used
#define BEAMCOUNT 2     //number of beams daisy chained together

/* Iniitialize DHT */ 
DHT dht(DHTPIN, DHTTYPE);

/* Iniitialize an instance of Beam */
Beam b = Beam(RSTPIN, IRQPIN, BEAMCOUNT);

/* pin definitions for PIR Motion Sensor */
int led = 7;                // choose the pin for the LED */
int pushButton = 4;              // choose the input pin (for pushbutton)
int pirState = LOW;             // we start, assuming no motion detected

void setup() {
    Particle.subscribe("hook-response/weather_pb", gotWeatherData, MY_DEVICES);
    Time.zone(+1);
	  dht.begin();
    Wire.begin();
    b.begin();
    pinMode(led, OUTPUT); // declare LED for PIR as output
    pinMode(D6, INPUT);     // declare sensor as input for PIR
    pinMode(pushButton, INPUT_PULLUP);      // declare pushbutton as input with pullup
}

void loop() {
    
        if (digitalRead(D6) == HIGH) {
            digitalWrite(led, HIGH);
            delay(1000);
            digitalWrite(led, LOW);
            Particle.publish("weather_pb");
            delay(180000);
        while (digitalRead(D6) == HIGH); // hang tight here until motion stops
    }
}

    void gotWeatherData(const char *name, const char *data) {
    
    float h = dht.getHumidity();
	float t = dht.getTempCelcius();
	float hi = dht.getHeatIndex();
	float dp = dht.getDewPoint();
	float k = dht.getTempKelvin();

    String str = String(data);
    String tempStr = tryExtractString(str, "<temp_c>", "</temp_c>");
    String condStr = tryExtractString(str, "<weather>", "</weather>");
    String humStr = tryExtractString(str, "<relative_humidity>", "</relative_humidity>");
    
    if (tempStr != NULL) {
    b.print("Hi! Es ist " + condStr + "   OUT:" + tempStr + String("'/") + humStr + "   IN:" + float2str(t,1) + "'/" + float2str(h,1) + "%");
    //b.print(Time.format(Time.now(), "%H:%M   ") + condStr + "   O:" + tempStr + String("'") + "   I:" + float2str(t,1) + "'   H:" + float2str(h,1) + "%");
    b.setSpeed(4);
    b.setLoops(20);
    b.play();
     }
}
// Returns any text found between a start and end string inside 'str'
// example: startfooend  -> returns foo
String tryExtractString(String str, const char* start, const char* end)
{
    if (str == NULL) {
        return NULL;
    }

    int idx = str.indexOf(start);
    if (idx < 0) {
        return NULL;
    }

    int endIdx = str.indexOf(end);
    if (endIdx < 0) {
        return NULL;
    }

    return str.substring(idx + strlen(start), endIdx);
}
