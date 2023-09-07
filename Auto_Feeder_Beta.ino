//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Servo.h>


#define DHTPIN 14            // Digital pin connected to the DHT11 data pin
#define DHTTYPE DHT11       // Set the sensor type to DHT11
DHT dht(DHTPIN, DHTTYPE);

const int servoPin = 15;   
#define relayPin 13
Servo myservo;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int humidityThreshold = 60;  // Humidity threshold (adjust as needed)
const int temperatureThreshold = 35;

unsigned long startTime;
unsigned long duration = 30 * 60 * 1000;
bool doorClosed = false;

void setup()
{
  myservo.attach(servoPin,600,2300);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);
  dht.begin();
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  startTime = millis();
  myservo.write(0);
}


void loop()
{
    // Read humidity from the DHT11 sensor
  float humidity = dht.readHumidity();

  // Read temperature from the DHT11 sensor in Celsius
  float temperature = dht.readTemperature();

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  if (humidity > humidityThreshold){
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, HIGH);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : ON");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Low");
  delay(3000);
  lcd.clear();
    if (elapsedTime >= duration) {
    
    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Memberi Pangan !!");
      myservo.write(180); // Move servo to close the door
      delay(500); // Wait for the servo to move (adjust as needed)
      myservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Waktu Reset!");
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
    delay(3000);
    lcd.clear();
  }

  } 
  else if (temperature > temperatureThreshold) {
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, LOW);
  // myservo.write(180);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : OFF");
  lcd.setCursor(0,1);
  lcd.print("Suhu : HIGH");
  delay(3000);
  lcd.clear();
   if (elapsedTime >= duration) {

    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Memberi Pangan !!");
      myservo.write(180); // Move servo to close the door
      delay(500); // Wait for the servo to move (adjust as needed)
      myservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Waktu Reset!");
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
    delay(3000);
    lcd.clear();
  }
} else {
  lcd.setCursor(0, 0);
  lcd.print("Humadity :");
  lcd.println(humidity);
  lcd.setCursor(0,1);
  lcd.print("Suhu :");
  lcd.println(temperature);
  digitalWrite(relayPin, LOW);
  // myservo.write(0);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lamp : OFF");
  lcd.setCursor(0,1);
  lcd.print("Suhu : Normal");
  delay(3000);
  lcd.clear();
   if (elapsedTime >= duration) {

    if (!doorClosed) {
      lcd.setCursor(0, 0);
      lcd.print("Memberi Pangan !!");
      myservo.write(180); // Move servo to close the door
      delay(500); // Wait for the servo to move (adjust as needed)
      myservo.write(0);
      doorClosed = true;
      lcd.clear();
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Waktu Reset!");
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
    delay(3000);
    lcd.clear();
  }
  
}

}
