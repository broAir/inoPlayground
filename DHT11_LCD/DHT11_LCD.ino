#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки
//#include <LiquidCrystal_PCF8574.h> // Подключение альтернативной библиотеки

LiquidCrystal_I2C lcd(0x27, 16, 2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой
//LiquidCrystal_PCF8574 lcd(0x27); // Вариант для библиотеки PCF8574

dht DHT;

// Pinouts
#define DHT11_PIN 7

#define RED_LED_PIN 11
#define BLUE_LED_PIN 13
#define GREEN_LED_PIN 12

#define HEATER_RELAY_PIN 1
#define FAN_RELAY_PIN 9

#define LED_LIGHT_RELAY_PIN 1
// System
#define DHT11_FAIL_MAX 10

#define CYCLE_LENGTH 5000 // 5 secound loop

int DHT11_FAIL_COUNTER = 0;

int TEMPERATURE_SETTING = 25;
int Dt_SETTING = 1;
bool TEMPSERVICE_LOCK = false;

void setup()
{
  // put your setup code here, to run once:
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(HEATER_RELAY_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(LED_LIGHT_RELAY_PIN, OUTPUT);

  Serial.begin(9600);

  lcd.init();          // Инициализация дисплея
  lcd.backlight();     // Подключение подсветки
  lcd.setCursor(0, 0); // Установка курсора в начало первой строки
  //lcd.print("Hello");       // Набор текста на первой строке
  lcd.setCursor(0, 1); // Установка курсора в начало второй строки
  //lcd.print("ArduinoMaster");
}

void loop()
{
  handleDHT();

  handleUserInput();

  delay(CYCLE_LENGTH);
}

void handleDHT() {
  int chk = DHT.read11(DHT11_PIN);

  if ((DHT.temperature > -25) && (DHT.humidity > -25)) {
    DHT11_FAIL_COUNTER = 0; // Reset the fail counter

    handleTemp(DHT.temperature);

    printDhtValues();
  }
  else {
    handleDHTfailure();
  }
}

void handleTemp(int temp) {
  int dt = temp - TEMPERATURE_SETTING;
  if (dt > Dt_SETTING) { // if dt is positive and bigger than the setting = we need to cool
    coolDown();
  }
  else if (-dt > Dt_SETTING) { // if dt is negative and less that the setting = we need to heat
    heat();
  }
  else { // if there is no significant difference between temps - turn off everything
    TEMPSERVICE_LOCK = false;

    turnOFF(BLUE_LED_PIN);
    turnOFF(FAN_RELAY_PIN);
    turnOFF(RED_LED_PIN);
    turnOFF(HEATER_RELAY_PIN);
  }
}

void heat() {
  if(TEMPSERVICE_LOCK)
    return;
  
  TEMPSERVICE_LOCK = true;
  turnON(HEATER_RELAY_PIN);
  turnON(RED_LED_PIN);
  Serial.print("Heating up ...");
}

void coolDown() {
  if(TEMPSERVICE_LOCK)
    return;
  
  TEMPSERVICE_LOCK = true;
  turnON(FAN_RELAY_PIN);
  turnON(BLUE_LED_PIN);
  Serial.print("Cooling down ...");
}

void printDhtValues() {
  int temp = DHT.temperature;
  int hum = DHT.humidity;
  Serial.print("Temperature = ");
  Serial.println(temp);
  Serial.print("Humidity = ");
  Serial.println(hum);

  if ((DHT.temperature > -25) && (DHT.humidity > -25)) {
    // Устанавливаем курсор на вторую строку и нулевой символ.
    lcd.setCursor(0, 1);
    // Выводим на экран количество секунд с момента запуска ардуины
    lcd.print("T:");
    lcd.print(temp);

    lcd.print(" Hmd:");
    lcd.print(hum);
    // Устанавливаем курсор на 1 строку и нулевой символ.
    lcd.setCursor(0, 0);
  }
}

void handleDHTfailure() {
  DHT11_FAIL_COUNTER++;
    if (DHT11_FAIL_COUNTER > DHT11_FAIL_MAX) {
      lcd.setCursor(0, 0);
      lcd.print("DHT11 FAIL");
    }
}

void handleUserInput() {
  char inpt[] = "";
  
  if (strcmp(inpt, "tf") == 0) {
      toggleFan();
  } 
  else if (strcmp(inpt, "tl") == 0) {
        toggleLight();
  }
  else {
  }
}

void toggleFan() {
  togglePin(FAN_RELAY_PIN);
  togglePin(BLUE_LED_PIN);
}

void toggleLight() {
  togglePin(LED_LIGHT_RELAY_PIN);
}

void setTemp(int t) {
  TEMPERATURE_SETTING = t;
}

void turnON(int pin) {
  digitalWrite(pin, HIGH);
}

void turnOFF(int pin) {
  digitalWrite(pin, LOW);
}

void togglePin(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}
