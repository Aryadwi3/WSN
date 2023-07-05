#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "RTClib.h"

// Setting rtc
RTC_DS3231 rtc;

RF24 radio(2, 8); // Create a Radio

struct data
{
  int tandanode;
  float temperature;
  float humidity;
  uint8_t jam;
  uint8_t menit;
  uint8_t detik;
};
struct data sentData;
struct data receivedData;
bool newData = false;

DHT dht(2, DHT11);

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(3, 0xF0F0F0F0E3LL); // Set the reading pipe address to match the writing pipe address of the sender
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  rtc.begin();
  dht.begin();
}

void loop()
{ 
  tugas();
  /*if (!rtc.begin()) 
  {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }
  //kode rtc
  DateTime now = rtc.now();
  terima_data(now);*/
}

void tugas(DateTime now)
{
  if (now.second() % 15 == 0 && now.second() % 15 < 3)
  {
    terima_data(now);
  }

  if (now.second() % 15 == 3 && now.second() % 15 < 6)
  {
    kirim_data11GW(now);
  }

  if (now.second() % 15 == 6 && now.second() % 15 < 9)
  {
    kirim_dataR(now);
  }

  if (now.second() % 15 == 9 && now.second() % 15 < 12)
  {
    terima_data(now);
  }
}
void terima_data(DateTime now)
{
  radio.startListening();

  if (radio.available())
  {
    radio.read(&receivedData, sizeof(receivedData));

    if (receivedData.tandanode == 11 || receivedData.tandanode == 12)
    {
      newData = true;
      Serial.print("Data from node: ");
      Serial.println(receivedData.tandanode);
      Serial.print("Temperature: ");
      Serial.print(receivedData.temperature);
      Serial.println("C");
      Serial.print("(");
      Serial.print(now.hour());
      Serial.print(":");
      Serial.print(now.minute());
      Serial.print(":");
      Serial.print(now.second());
      Serial.println(")");
    }
  }
}

void kirim_data11GW(DateTime now)
{
  radio.stopListening();

  sentData.tandanode = 01;
  sentData.temperature = dht.readTemperature();
  sentData.humidity = dht.readHumidity();

  if (isnan(sentData.humidity) || isnan(sentData.temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  radio.write(&sentData, sizeof(sentData));
  Serial.println("Kirim Data 01 ke Gateway");
  Serial.print("Temperature: ");
  Serial.print(sentData.temperature);
  Serial.println("C");
  Serial.print("(");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println(")");
  delay(500);
  newData = false;
}
void kirim_dataR(DateTime now){
  radio.stopListening();

  sentData.tandanode = 01;
  sentData.temperature = dht.readTemperature();
  sentData.humidity = dht.readHumidity();

  if (isnan(sentData.humidity) || isnan(sentData.temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  radio.write(&sentData, sizeof(sentData));
  Serial.println("Kirim Data R ke Gateway");
  Serial.print("Temperature: ");
  Serial.print(sentData.temperature);
  Serial.println("C");
  Serial.print("(");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println(")");
  delay(500);
  newData = false;
}
