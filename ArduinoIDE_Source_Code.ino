#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Pin configuration
#define LCD_ADDRESS 0x27       // I2C address for LCD 1602A
#define PIR_PIN 12             // GPIO 12 for PIR sensor
#define LED_RED_PIN 16         // GPIO 16 for Red LED
#define LED_GREEN_PIN 17       // GPIO 17 for Green LED
#define BUZZER_PIN 14          // GPIO 14 for Buzzer
#define RELAY_PIN 15           // GPIO 15 for Relay (Solenoid Control)

// RFID pin configuration
#define RST_PIN 4              // GPIO 4 for RFID RST
#define SS_PIN 5               // GPIO 5 for RFID SDA
#define SCK_PIN 18             // GPIO 18 for RFID SCK
#define MOSI_PIN 2            // GPIO 2 for RFID MOSI
#define MISO_PIN 19            // GPIO 19 for RFID MISO

// LCD Pin Configuration (I2C)
#define SDA_PIN 21             // GPIO 21 for SDA (I2C data)
#define SCL_PIN 22             // GPIO 22 for SCL (I2C clock)

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);   // LCD configuration
MFRC522 rfid(SS_PIN, RST_PIN);                // RFID configuration

// Variables to track RFID detection and PIR state
bool rfidDetected = false;
unsigned long rfidDetectedTime = 0;           // Store the time when RFID is detected
const unsigned long PIR_BLOCK_TIME = 60000;   // 20 seconds block time for PIR after RFID detection

// Valid UID for the system (example)
String validUID = "7361030";  // Replace with actual valid UID

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LCD
  Wire.begin(SDA_PIN, SCL_PIN); // Set SDA and SCL pins
  lcd.begin(16, 2);              // Initialize 16x2 LCD
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home Sec");

  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();

  // Initialize other pins
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Initially, solenoid is locked
  digitalWrite(RELAY_PIN, HIGH);
  delay(2000);
  lcd.clear();
}

void loop() {
  // Check if a new RFID card is detected
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("RFID Card Detected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card UID:");

    // Read UID of the RFID card
    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidStr += String(rfid.uid.uidByte[i], HEX);
    }

    // Display the UID on the LCD
    lcd.setCursor(0, 1);
    lcd.print(uidStr);
    Serial.print("UID: ");
    Serial.println(uidStr);

    // Check if UID matches the valid UID
    if (uidStr == validUID) {
      // Green LED turns on if card detected
      digitalWrite(LED_GREEN_PIN, HIGH);

      // Unlock solenoid (door open)
      digitalWrite(RELAY_PIN, LOW);   // Relay LOW = solenoid open, door unlocked
      delay(5000); // Keep door open for 3 seconds
      digitalWrite(RELAY_PIN, HIGH);  // Relay HIGH = solenoid locked, door locked

      // Stop RFID detection
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      rfidDetected = true;
      rfidDetectedTime = millis();  // Update RFID detection time
    } else {
      // If UID is not valid, show a message on LCD and turn on buzzer
      digitalWrite(LED_RED_PIN, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Card!");
      digitalWrite(BUZZER_PIN, HIGH);  // Turn on the buzzer for invalid card
      delay(2000); // Show "Invalid Card!" for 2 seconds
      digitalWrite(BUZZER_PIN, LOW);   // Turn off the buzzer after 2 seconds
    }
  }

  // Check PIR sensor if motion is detected
  int pirStatus = digitalRead(PIR_PIN);

  // If motion detected, but within 60 seconds after RFID detection, do not trigger buzzer
  if (pirStatus == HIGH && rfidDetected && (millis() - rfidDetectedTime < PIR_BLOCK_TIME)) {
    // PIR is blocked for 20 seconds after RFID card detection, so no alert is triggered
    digitalWrite(LED_GREEN_PIN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected, ");
    lcd.setCursor(0, 1);
    lcd.print("Home Secured");
  } else if (pirStatus == HIGH) {
    // Motion detected and no card detected or timer expired
    digitalWrite(LED_RED_PIN, HIGH);
    Serial.println("Motion Detected!");

    // Trigger the buzzer for motion detected
    lcd.setCursor(0, 1);
    lcd.print("Intruder Alert! ");
    digitalWrite(BUZZER_PIN, HIGH); // Trigger buzzer
    delay(2000); // Duration for buzzer sound
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    // Home secured or no motion detected
    lcd.setCursor(0, 1);
    lcd.print("Home Secured    ");
  }

  // Reset Green LED after card scan is completed
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);

  delay(500); // Delay to avoid flicker
}
