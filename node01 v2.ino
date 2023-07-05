#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "RTClib.h"

RTC_DS3231 rtc;

RF24 radio(7, 8); // Create a Radio

struct data
{
  int tandanode;
  float temperature;
  float humidity;
  int jam;
  int menit;
  int detik;
};

struct data sentData;
struct data receivedData;
bool newData = false;

DHT dht(2, DHT11);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(0xF0F0F0F0E3LL); // Set the writing pipe address to match the reading pipe address of the receiver
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.openReadingPipe(2, 0xF0F0F0F0E2LL);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  rtc.begin();
}

void loop()
{
  if (!rtc.begin()) 
  {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }

  DateTime now = rtc.now();
  tugas(now);
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

  if (now.second() % 15 == 12 && now.second() % 15 < 15)
  {
    kirim_data12GW(now);
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
      Serial.print(receivedData.tandanode);
      Serial.print(" Temperature: ");
      Serial.print(receivedData.temperature);
      Serial.print(", ");
      Serial.print(", Humidity: ");
      Serial.print(receivedData.humidity);
      Serial.print(" (");
      Serial.print(now.hour());
      Serial.print(":");
      Serial.print(now.minute());
      Serial.print(":");
      Serial.print(now.second());
      Serial.println(") ");
    }
  }
}

void kirim_data11GW(DateTime now)
{
  radio.stopListening();

  sentData.tandanode = 11;
  sentData.temperature = receivedData.temperature;
  sentData.humidity = receivedData.humidity;

  if (isnan(sentData.humidity) || isnan(sentData.temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  radio.write(&sentData, sizeof(sentData));
  Serial.println("Kirim Data 11 ke Gateway");
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

void kirim_data12GW(DateTime now)
{
  radio.stopListening();

  sentData.tandanode = 12;
  sentData.temperature = receivedData.temperature;
  sentData.humidity = receivedData.humidity;

  if (isnan(sentData.humidity) || isnan(sentData.temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  radio.write(&sentData, sizeof(sentData));
  Serial.println("Kirim Data 12 ke Gateway");
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

void kirim_dataR(DateTime now)
{
  radio.stopListening();

  sentData.tandanode = 13;
  sentData.temperature = dht.readTemperature();
  sentData.humidity = dht.readHumidity();

  if (isnan(sentData.humidity) || isnan(sentData.temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  radio.write(&sentData, sizeof(sentData));
  Serial.print("Kirim Data R ke Gateway");
  Serial.print(" Temperature: ");
  Serial.print(sentData.temperature);
  Serial.print(" C, ");
  Serial.print(" Humidity: ");
  Serial.print(sentData.humidity);
  Serial.print(" (");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println(")");
  delay(500);
  newData = false;
}
