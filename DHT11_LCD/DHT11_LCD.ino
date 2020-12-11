#include <time.h>
#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Подключение библиотеки
//#include <LiquidCrystal_PCF8574.h> // Подключение альтернативной библиотеки

LiquidCrystal_I2C lcd(0x27, 16, 2); // Указываем I2C адрес (наиболее распространенное значение), а также параметры экрана (в случае LCD 1602 - 2 строки по 16 символов в каждой
//LiquidCrystal_PCF8574 lcd(0x27); // Вариант для библиотеки PCF8574

dht DHT;

#define DHT11_PIN 7

int TEMPERATURE_Setting = 25;
int TEMPERATURE_Delta = 1;
bool TEMPSERVICE_LOCK = false;

int RED_LED_PIN = 11;
int BLUE_LED_PIN = 13;
int GREEN_LED_PIN = 12;

int DHT11_Fail_Count = 0;
int DHT11_Fail_Max = 10;
void setup()
{
  // put your setup code here, to run once:
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  srand(time(NULL));

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

  // digitalWrite(12, HIGH);
  // delay(rand()%5*10);

  // digitalWrite(12, LOW);

  // delay(rand()%5*10);

  int chk = DHT.read11(DHT11_PIN);

  if ((DHT.temperature > -25) && (DHT.humidity > -25))
  {

    checkTemp(DHT.temperature);

    printDhtValues();
    DHT11_Fail_Count = 0;
  }
  else
  {
    DHT11_Fail_Count++;
    if (DHT11_Fail_Count > DHT11_Fail_Max)
    {

      lcd.setCursor(0, 0);
      lcd.print("DHT11 FAIL");
    }
  }
  delay(5000);
}

void checkTemp(int temp)
{
  int dt = TEMPERATURE_Setting - temp;
  if (dt > TEMPERATURE_Delta)
  {
    heat();
  }
  else if (dt < -TEMPERATURE_Delta)
  {
    coolDown();
  }
  else
  {

    TEMPSERVICE_LOCK = false;
    digitalWrite(BLUE_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
}

void heat()
{
  if (!TEMPSERVICE_LOCK)
  {
    TEMPSERVICE_LOCK = true;
    togglePin(RED_LED_PIN);
  }
}
void coolDown()
{
  if (!TEMPSERVICE_LOCK)
  {
    TEMPSERVICE_LOCK = true;
    togglePin(BLUE_LED_PIN);
    Serial.print("Cooling down");
  }
}

void togglePin(int pin)
{
  digitalWrite(pin, !digitalRead(pin));
}

void printDhtValues()
{
  int temp = DHT.temperature;
  int hum = DHT.humidity;
  Serial.print("Temperature = ");
  Serial.println(temp);
  Serial.print("Humidity = ");
  Serial.println(hum);

  if ((DHT.temperature > -25) && (DHT.humidity > -25))
  {
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
