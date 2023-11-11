#include "ESC.h"
#define LED_PIN (13)                    // Pin for the LED 
#define POT_PIN (A0)                    // Analog pin used to connect the potentiometer

ESC myESC (9, 1000, 2000, 500);         // ESC_Name (PIN, Minimum Value, Maximum Value, Arm Value)


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int val;                                // variable to read the value from the analog pin

volatile unsigned long pulseCount = 0; // Jumlah pulsa interrupt
unsigned long previousMillis = 0;     // Waktu sebelumnya saat menghitung pulsa
unsigned long rpm = 0;                // Variabel untuk menyimpan hasil perhitungan RPM

const int interruptPin = 2; // Pin yang terhubung dengan interrupt (INT0 pada Arduino Uno)
int proximity_1 = 5;
int proximity_2 = 6;
int start_pin = 7;
String direction_winch = "forward";
String direction_winch_prev = "forward";

float esc_speed = 0;
float esc_speed_prescaler = 1; // angka ini yang dikalibrasi dengan konstruksi mekanikal


void setup() {
  myESC.arm();  
  pinMode(proximity_1, INPUT_PULLUP);
  pinMode(proximity_2, INPUT_PULLUP);
  pinMode(start_pin, INPUT_PULLUP);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countPulses, FALLING); // Mengaktifkan interrupt pada falling edge
  lcd.init();
}

void loop() {
  unsigned long currentMillis = millis();

  // Menghitung RPM setiap 100 milidetik (0,1 detik)
  if (currentMillis - previousMillis >= 100) {
    detachInterrupt(digitalPinToInterrupt(interruptPin)); // Matikan interrupt sementara
    rpm = (pulseCount * 60 * 10) / 11; // Menghitung RPM (misalnya, 110 pulsa per putaran)
    pulseCount = 0; // Reset jumlah pulsa
    previousMillis = currentMillis; // Simpan waktu sebelumnya
    attachInterrupt(digitalPinToInterrupt(interruptPin), countPulses, FALLING); // Aktifkan interrupt kembali
    Serial.println(rpm); // Tampilkan RPM di Serial Monitor
  }

  if (digitalRead(proximity_1) == HIGH){
    direction_winch = "forward";
  }

  
  if (digitalRead(proximity_2) == HIGH){
    direction_winch = "reverse";
  }

  esc_speed = rpm * esc_speed_prescaler;
  if (esc_speed > 500){
    esc_speed = 500;
  }
  if (digitalRead(start_pin) == HIGH){
  
  if (direction_winch == "forward"){
    myESC.speed(1500 + esc_speed);
  }
  
  if (direction_winch == "reverse"){
    myESC.speed(1500 - esc_speed);
  }

  if (direction_winch == direction_winch_prev){
    myESC.arm();  
  }
  }
  
  direction_winch_prev = direction_winch;
}

void countPulses() {
  pulseCount++; // Tambahkan jumlah pulsa saat interrupt terjadi
}
