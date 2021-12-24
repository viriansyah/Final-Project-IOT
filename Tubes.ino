// DHT dan ESP8266
#include <ESP8266WiFi.h>
#include <DHT.h>

//firebase
#include "FirebaseESP8266.h"
#define FIREBASE_HOST "fir-suhu-77e7a-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "uawPDCmRZcbX0UGAiHaRWESujzHb95FIal5HqoZm"
FirebaseData firebaseData;

//blynk
#define BLYNK_PRINT Serial   
#include <SPI.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "szKxwE7ZsedxOI_g_AYJdPGuSU6-0F3A"; //code auth blynk

//Gsheet
#include "TRIGGER_WIFI.h"               
#include "TRIGGER_GOOGLESHEETS.h"  
char column_name_in_sheets[ ][20] = {"value1","value2"};                        
String Sheets_GAS_ID = "AKfycbxwrZD3dfxTUQdOWIVd1YMo21G6uoLUsq_eSTxPhkruoTKl225ynFDmzVuJNDGxhiDr";
int No_of_Parameters = 2;

//Setting wifi
char ssid[] = "SIVIE-HP";
char pass[] = "dimcan@1974";

//Setting Pin
#define DHTPIN D4          // Digital pin 4
#define enA D8
#define in1 D7
#define in2 D6

#define DHTTYPE DHT11     // DHT 11

//setting speed dinamo
int speedCar = 800;
int speedCar2 = 0;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WidgetLCD lcd(V7);

void setup()
{
  pinMode(in1, OUTPUT); 
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  Serial.begin(9600); // See the connection status in Serial Monitor
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String S;
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Kirim Gsheet
  Data_to_Sheets(No_of_Parameters,  t,  h);

  //Kirim Blynk
  Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  Blynk.virtualWrite(V6, t);  //V6 is for Temperature

  //Kirim Firebase
  if(Firebase.setFloat(firebaseData,"/Hasil_Suhu/Suhu", t)){
    Serial.println("suhu terkirim");
  }else{
    Serial.println("suhu tidak terkirim");
    Serial.println("karena : "+firebaseData.errorReason());
  }

   if(Firebase.setFloat(firebaseData,"/Hasil_Kelembapan/kelembapan", h)){
    Serial.println("kelembaban terkirim");
  }else{
    Serial.println("kelembaban tidak terkirim");
    Serial.println("karena : "+firebaseData.errorReason());
  }

  //logika dinamo
  if (t > 33)
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, speedCar);
    lcd.clear();
    lcd.print(0,0,"Status Fan");
    lcd.print(0,1,">> Menyala");
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, speedCar2);
    lcd.clear();
    lcd.print(0,0,"Status Fan");
    lcd.print(0,1,">> Mati");
  } 

  //Jeda Waktu
  delay(1000);
  timer.run();
}
