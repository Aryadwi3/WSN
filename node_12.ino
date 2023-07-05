#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
#include <Wire.h>
#include "RTClib.h"

//const uint64_t kirim11 = 0xF0F0F0F0E1LL;

RTC_DS3231 rtc;

RF24 rf24(7, 8);

struct data
{
  int tandanode;
  float temperature; // Menggunakan tipe data float untuk suhu
  float humidity;    // Menggunakan tipe data float untuk kelembaban
  uint8_t jam;
  uint8_t menit;
  uint8_t detik;
};
struct data sentData;

DHT dht(2, DHT11);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  rf24.begin();
  rf24.setDataRate(RF24_2MBPS);
  rf24.openWritingPipe(0xF0F0F0F0E1LL);
  rf24.setPALevel(RF24_PA_MIN);
  rf24.stopListening(); // Mengubah mode radio menjadi pengiriman data
  rtc.begin();
}

void loop()
{
  DateTime now = rtc.now();

  // Membaca data suhu dan kelembaban dari sensor DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  sentData.tandanode = 12;
  sentData.temperature = temperature;
  sentData.humidity = humidity;
  sentData.jam = now.hour();
  sentData.menit = now.minute();
  sentData.detik = now.second();

  rf24.write(&sentData, sizeof(sentData));

  Serial.print("Tandanode: ");
  Serial.println(sentData.tandanode);
  Serial.print("Temperature: ");
  Serial.print(sentData.temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(sentData.humidity);
  Serial.println(" %");
  Serial.print("Jam: ");
  Serial.println(sentData.jam);
  Serial.print("Menit: ");
  Serial.println(sentData.menit);
  Serial.print("Detik: ");
  Serial.println(sentData.detik);
  Serial.println();

  rf24.write(&sentData, sizeof(sentData));

  delay(1000); // Mengirim data setiap 15 detik
}
