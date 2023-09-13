

//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Define your WiFi credentials here
const char* ssid = "IoT-Vernszz";
const char* password = "yangtawumahtawu";


#define DHTPIN 14            // Digital pin connected to the DHT11 data pin
#define DHTTYPE DHT11       // Set the sensor type to DHT11
DHT dht(DHTPIN, DHTTYPE);

#define servoPin 15   
#define relayPin 13
#define servoDoorPin 12  
Servo doorservo;
Servo myservo;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int humidityThreshold = 50;  // Humidity threshold (adjust as needed)
const int temperatureThreshold = 25;

unsigned long startTime;
unsigned long duration = 0 * 20 * 1000;
bool doorClosed = false;



void setup()
{
  // Initialize serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
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



  myservo.attach(servoPin,600,2300);
  doorservo.attach(servoDoorPin,600,2300);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  dht.begin();
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  startTime = millis();
  myservo.write(0);
  doorservo.write(0);
  lcd.setCursor(0, 0);
  lcd.print("Auto Feeder ON!!");
  lcd.setCursor(0, 1);
  lcd.print("By 4 Barudaks!!!");
  delay(3000);
}


void loop()
{
  getDataFromAPI();
    // Read humidity from the DHT11 sensor
  float humidity = dht.readHumidity();

  // Read temperature from the DHT11 sensor in Celsius
  float temperature = dht.readTemperature();

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  if (humidity > humidityThreshold && temperature > temperatureThreshold){
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, LOW);
  doorservo.write(0);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : Off");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Hangat!!");
  delay(2000);
  lcd.clear();
  } 
  else if (humidity < humidityThreshold && temperature < temperatureThreshold) {
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, HIGH);
  doorservo.write(180);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : ON");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Dingin!");
  delay(2000);
  lcd.clear();
} else if (humidity > humidityThreshold && temperature < temperatureThreshold){
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, HIGH);
  doorservo.write(180);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : ON");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Dingin!!");
  delay(2000);
  lcd.clear();
  
} else if (humidity < humidityThreshold && temperature > temperatureThreshold){
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, LOW);
  doorservo.write(0);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : OFF");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Hangat!!");
  delay(2000);
  lcd.clear();
} else {
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, LOW);
  doorservo.write(0);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : OFF");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Normal");
  delay(2000);
  lcd.clear();
}

  if (elapsedTime >= duration) {
    
    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Memberi Pangan !!");
      myservo.write(180);
      doorservo.write(360);
      delay(2000); // Wait for the servo to move (adjust as needed)
      myservo.write(0);
      doorservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Waktu Reset!!");
    delay(2000); // Wait for 2 seconds before resetting the timer
    startTime = millis(); // Reset the timer
    doorClosed = false;
  } else {
    unsigned long remainingTime = duration - elapsedTime;
    int hours = remainingTime / 3600000;
    int minutes = (remainingTime % 3600000) / 60000;
    int seconds = (remainingTime % 60000) / 1000;

    lcd.setCursor(2, 0);
    lcd.print("Timer Pintu");
    lcd.setCursor(2, 1);
    lcd.print(String(hours) + ":" + String(minutes) + ":" + String(seconds));
    delay(2000);
    lcd.clear();
  }

}

void getDataFromAPI() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client; // Create a WiFiClient object

        HTTPClient http;

        // API URL
        String url = "http://192.168.43.39:3062/users";

        // Open connection to the API
        http.begin(client, url);

        // Perform a GET request
        int httpCode = http.GET();

        // If the request is successful
        if (httpCode == HTTP_CODE_OK) {
            String response = http.getString();
            Serial.println(response); // Print the response to the Serial Monitor

            // Parse the JSON response to extract the 'duration' value
            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, response);

            if (!error && doc.containsKey("duration")) {
                duration = doc["duration"].as<unsigned long>();
                Serial.print("Duration from API: ");
                Serial.println(duration);
            } else {
                Serial.println("Error parsing JSON or 'duration' not found in response");
            }
        } else {
            Serial.println("Failed to make HTTP request");
        }

        // Close the HTTP connection
        http.end();
    }
}
