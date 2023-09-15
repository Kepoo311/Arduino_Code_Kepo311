#include <ESP32Servo.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
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
const char* ssid = "dns";
const char* password = "denara99";
String baseUrl = "https://apiautofeeder.k4project.online";

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int humidityThreshold = 50;
const int temperatureThreshold = 25;

unsigned long startTime;
unsigned long duration;
bool doorClosed = false;
unsigned long previousTimeFromAPI = 0;
unsigned long currentTimeFromAPI = 0;
unsigned long jedabukamakan;
float suhu;
float kelembapan;
int id = 1;
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

void HumSuh();
void Timer();
void connectWifi();
void getDataFromAPI();
// void PostDataToApi();
void updateDHTData(int id, float suhu, float kelembapan);

void setup() {
  Serial.begin(9600);
  getDataFromAPI();
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
  currentTime = millis();
  elapsedTime = currentTime - startTime;
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  suhu = temperature;
  kelembapan = humidity;
  // PostDataToApi();
  getDataFromAPI();
  HumSuh();
  updateDHTData(id, suhu, kelembapan);
  Timer();
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

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  IPAddress ip = WiFi.localIP();
  Serial.println();
  Serial.println("Connected to WiFi network.");
  Serial.print("To access the server, connect with Telnet client to ");
  Serial.print(ip);
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
          duration = waktumakan * 60 * 1000;
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


void updateDHTData(int id, float suhu, float kelembapan) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Membangun URL dengan nilai dari variabel
    String url = baseUrl + "users/updatedht/" + String(id) + "/" + String(suhu) + "/" + String(kelembapan);

    // Mulai koneksi HTTP
    http.begin(url);

    // Tetapkan metode PUT
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.PUT("");

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Update successful:");
      Serial.println(payload);
      // Tambahkan tindakan lain jika diperlukan
    } else {
      Serial.print("HTTP PUT failed, error code: ");
      Serial.println(httpCode);
    }

    // Akhiri koneksi HTTP
    http.end();
  } else {
    Serial.println("Not connected to WiFi");
  }
}

// void PostDataToApi() {
//     if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     String url = "https://apiautofeeder.k4project.online/updatedht/" + String(id) + "/" + String(suhu) + "/" + String(kelembapan);

//     http.begin(url);

//     int httpCode = http.PUT(""); // Tidak perlu mengirim data POST dalam kasus ini

//     if (httpCode == 200) {
//       Serial.println("Update successful!");
//     } else {
//       Serial.println("Update failed!");
//     }

//     http.end();
//   }
// }
