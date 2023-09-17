#include <ESP32Servo.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>//
#include <LiquidCrystal_I2C.h>

#define servoPin 16
#define relayPin 13
#define servoDoorPin 12
#define servoDoorPin1 17
#define DHTPIN 0
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

Servo doorservo;
Servo doorservo1;
Servo myservo;

// Masukkan kredensial WiFi Anda di sini
// const char* ssid = "dns";
// const char* password = "denara99";

// const char* ssid = "IoT-Vernszz";
// const char* password = "yangtawumahtawu";

const char* ssid = "Kepo";
const char* password = "PKL12311";

String baseUrl = "https://apiautofeeder.k4project.online/users/updatedht/1";

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int humidityThreshold = 50;
const int temperatureThreshold = 25;

unsigned long startTime;
unsigned long duration;
unsigned long Offduration = 12 * 60 * 60 * 1000;//durasi untuk timer offline
bool doorClosed = false;
unsigned long previousTimeFromAPI = 0;
unsigned long currentTimeFromAPI = 0;
unsigned long jedabukamakan;
float suhu;
float kelembapan;
int id = 1;
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  bool isOnline = false; // Tambahkan variabel status WiFi

void HumSuh();
void Timer();
void connectWifi();
void getDataFromAPI();
// void PostDataToApi();
void updateDHTData(float suhu, float kelembapan);

void setup() {
  Serial.begin(9600);
  // getDataFromAPI();
  connectWifi();
  myservo.attach(servoPin, 600, 2300);
  doorservo.attach(servoDoorPin, 600, 2300);
  doorservo1.attach(servoDoorPin1, 600, 2300);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  dht.begin();
  lcd.init();
  lcd.backlight();
  startTime = millis();
  myservo.write(0);
  doorservo.write(0);
  doorservo1.write(0);
  lcd.setCursor(0, 0);
  lcd.print("Auto Feeder ON!!");
  lcd.setCursor(0, 1);
  lcd.print("By 4 Barudaks!!!");
  delay(3000);
}

void loop() {
  if (isOnline){
  currentTime = millis();
  elapsedTime = currentTime - startTime;
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  suhu = temperature;
  kelembapan = humidity;
  // PostDataToApi();
  getDataFromAPI();
  HumSuh();
  updateDHTData(suhu, kelembapan);
  Timer();
  } else {
    int loopCount = 0;
    loopCount++;
    currentTime = millis();
    elapsedTime = currentTime - startTime;
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    HumSuh();
    TimerOffline();

    if(loopCount >= 50){
      lcd.print("Trying Connecting To Wifi!!");
      connectWifi();
      loopCount = 0;
    }
  }
}

void HumSuh() {
  if (kelembapan > humidityThreshold && suhu > temperatureThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity :");
    lcd.println(kelembapan);
    lcd.setCursor(0, 1);
    lcd.print("Temperature :");
    lcd.println(suhu);
    digitalWrite(relayPin, LOW);
    doorservo.write(0);
    doorservo1.write(0);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp : Off");
    lcd.setCursor(0, 1);
    lcd.print("Temperature : Warm!!");
    delay(1000);
    lcd.clear();
  } else if (kelembapan < humidityThreshold && suhu < temperatureThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity :");
    lcd.println(kelembapan);
    lcd.setCursor(0, 1);
    lcd.print("Temperature :");
    lcd.println(suhu);
    digitalWrite(relayPin, HIGH);
    doorservo.write(180);
    doorservo1.write(180);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp : ON");
    lcd.setCursor(0, 1);
    lcd.print("Temperature : Cold!");
    delay(1000);
    lcd.clear();
  } else if (kelembapan > humidityThreshold && suhu < temperatureThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity :");
    lcd.println(kelembapan);
    lcd.setCursor(0, 1);
    lcd.print("Temperature :");
    lcd.println(suhu);
    digitalWrite(relayPin, HIGH);
    doorservo.write(180);
    doorservo1.write(180);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp : ON");
    lcd.setCursor(0, 1);
    lcd.print("Temperature : Cold!!");
    delay(1000);
    lcd.clear();
  } else if (kelembapan < humidityThreshold && suhu > temperatureThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity :");
    lcd.println(kelembapan);
    lcd.setCursor(0, 1);
    lcd.print("Temperature :");
    lcd.println(suhu);
    digitalWrite(relayPin, LOW);
    doorservo.write(0);
    doorservo1.write(0);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp : OFF");
    lcd.setCursor(0, 1);
    lcd.print("Temperature : Warm!!");
    delay(1000);
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Humidity :");
    lcd.println(kelembapan);
    lcd.setCursor(0, 1);
    lcd.print("Temperature :");
    lcd.println(suhu);
    digitalWrite(relayPin, LOW);
    doorservo.write(0);
    doorservo1.write(0);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lamp : OFF");
    lcd.setCursor(0, 1);
    lcd.print("Temperature : Normal");
    delay(1000);
    lcd.clear();
  }
}

void Timer() {
  if (elapsedTime >= duration) {
    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Feeding !!");
      myservo.write(180);
      delay(jedabukamakan);
      myservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    lcd.setCursor(0, 0);
    lcd.print("Resetting Timer!!");
    delay(2000);
    startTime = millis();
    doorClosed = false;
  } else if (currentTimeFromAPI != previousTimeFromAPI) {
    Serial.println("Time from API has changed.");
    lcd.setCursor(2, 0);
    lcd.clear();
    lcd.print("Time Changed!!");
    delay(1000);
    lcd.clear();
    previousTimeFromAPI = currentTimeFromAPI;
    startTime = millis();
  } else {
    unsigned long remainingTime = duration - elapsedTime;
    int hours = remainingTime / 3600000;
    int minutes = (remainingTime % 3600000) / 60000;
    int seconds = (remainingTime % 60000) / 1000;
    lcd.setCursor(2, 0);
    lcd.print("Door Timer");
    lcd.setCursor(2, 1);
    lcd.print(String(hours) + ":" + String(minutes) + ":" + String(seconds));
    delay(2000);
    lcd.clear();
  }
}

void TimerOffline() {
  if (elapsedTime >= Offduration) {
    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Feeding !!");
      myservo.write(180);
      delay(2000);
      myservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    lcd.setCursor(0, 0);
    lcd.print("Resetting Timer!!");
    delay(2000);
    startTime = millis();
    doorClosed = false;
  } else {
    unsigned long remainingTime = Offduration - elapsedTime;
    int hours = remainingTime / 3600000;
    int minutes = (remainingTime % 3600000) / 60000;
    int seconds = (remainingTime % 60000) / 1000;
    lcd.setCursor(2, 0);
    lcd.print("Door Timer");
    lcd.setCursor(2, 1);
    lcd.print(String(hours) + ":" + String(minutes) + ":" + String(seconds));
    delay(2000);
    lcd.clear();
  }
}


void connectWifi() {
   WiFi.begin(ssid, password);
  int attemptCount = 0;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attemptCount++;
    
    if (attemptCount >= 25) { // Jika gagal terhubung dalam 25 percobaan, beralih ke mode offline
      isOnline = false;
      Serial.println("Switching to offline mode");
      break;
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isOnline = true; // Berhasil terhubung ke WiFi
    Serial.println("Connected to WiFi");
    IPAddress ip = WiFi.localIP();
    Serial.println();
    Serial.println("Connected to WiFi network.");
    Serial.print("To access the server, connect with Telnet client to ");
    Serial.print(ip);
  }
}

void getDataFromAPI() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    if (http.begin("https://apiautofeeder.k4project.online/users/1")) {
      int httpCode = http.GET();
      if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument jsonDoc(1024);
        DeserializationError error = deserializeJson(jsonDoc, payload);
        if (!error) {
          int id = jsonDoc[0]["id"];
          int jedabuka = jsonDoc[0]["jedabuka"];
          int waktumakan = jsonDoc[0]["waktuMakan"];
          Serial.print("ID: ");
          Serial.println(id);
          Serial.print("Jedabuka: ");
          Serial.println(jedabuka);
          Serial.print("Waktu Makan: ");
          Serial.println(waktumakan);
          duration = waktumakan * 60 * 60 * 1000;
          currentTimeFromAPI = waktumakan;
          jedabukamakan = jedabuka * 1000;
        } else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.println("HTTP GET failed");
      }
      http.end();
    } else {
      Serial.println("HTTP connection failed");
    }
  }
}


void updateDHTData(float suhu, float kelembapan) {
  HTTPClient http;
  String response;

  StaticJsonDocument<200> buff;
  String jsonParams;
  buff["suhu"] = suhu;
  buff["kelembapan"] = kelembapan;

  serializeJson(buff, jsonParams);
  Serial.println(jsonParams);

  http.begin(baseUrl);
  http.addHeader("Content-Type", "application/json");
  http.PUT(jsonParams);
  response = http.getString();
  Serial.println(response);
}
