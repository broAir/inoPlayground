#include <Wire.h>
#include <SoftwareSerial.h>
#include <dht.h>

#define DEBUG 1

#define DHT11_PIN 7

#define RED_LED_PIN 11
#define BLUE_LED_PIN 13
#define GREEN_LED_PIN 12

#define HEATER_RELAY_PIN 10
#define FAN_RELAY_PIN 9

#define LED_LIGHT_RELAY_PIN 7

bool doUpdateStatus = false;

void updateBluetoothMonitor(SoftwareSerial &bluetooth, dht &btdht)
{
    if (doUpdateStatus)
    {
        int temp = btdht.temperature;
        int hum = btdht.humidity;

        Serial.print(digitalRead(HEATER_RELAY_PIN));
        Serial.print(digitalRead(FAN_RELAY_PIN));
        Serial.print(digitalRead(LED_LIGHT_RELAY_PIN));
        
        int isHeating = digitalRead(HEATER_RELAY_PIN);
        int isFanning = digitalRead(FAN_RELAY_PIN);
        int isLED = digitalRead(LED_LIGHT_RELAY_PIN);
        
        static unsigned long lastRefreshTime = 0;

        if (millis() - lastRefreshTime >= 1000)
        {
            lastRefreshTime += 1000;

            if (doUpdateStatus && (temp > -25) && (hum > -25))
            {
                // Every update there are temperatures and water pH level sent coded into binary form of:
                // 't',
                // integer part of value in Celcius of first termometer,
                // fractional part of value in Celcius of first termometer,
                // integer part of value in Celcius of second termometer,
                // fractional part of value in Celcius of second termometer,
                // 'w'
                // integer value of water pH level,
                // fractional part of water pH level.

                bluetooth.write('t');
                bluetooth.write(static_cast<byte>(temp));
                bluetooth.write(static_cast<byte>(hum));
                
                //bluetooth.write('f');
                bluetooth.write(static_cast<byte>(isFanning));
                
                //bluetooth.write('h');
                bluetooth.write(static_cast<byte>(isHeating));
                
                //bluetooth.write('l');
                bluetooth.write(static_cast<byte>(isLED));
            }
        }
    }
}

void toggleBluetoothMonitor(bool update)
{
    doUpdateStatus = update;
    if (update)
    {
        digitalWrite(GREEN_LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(GREEN_LED_PIN, LOW);
    }
}
