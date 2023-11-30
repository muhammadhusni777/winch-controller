#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define prox1 3 //ujung kanan
#define prox2 4 //ujung kiri

#define sw 10 // toggle switch
#define hijau A1 // lampu
#define fwd 9
#define off 7

#define CLK 9
#define DT 2

const int pwmPin = 11;  // Pin PWM pada Arduino Uno
const int rpmTarget = 1000;  // Kecepatan target dalam RPM

const int sensorPin = 2;  // Pin digital yang terhubung ke output sensor
volatile unsigned long count = 0;  // Variabel untuk menyimpan jumlah putaran
const float jarakAntarGigi = 2.0;  // Jarak antara setiap gigi dalam sentimeter

unsigned long pulse_per_second = 0;

unsigned long time_per_second;
unsigned long time_per_second_prev;

int currentStateCLK;
int lastStateCLK;
int p1;
int p2;
bool p1_lock;
bool p2_lock;

float counter = 0;

bool direction_motor;
bool initializing;

bool winch_direction = false;
bool winch_direction_prev = false;
int sw_prev;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(prox1,INPUT_PULLUP);
  pinMode(prox2,INPUT_PULLUP);
  pinMode(off,INPUT_PULLUP);

  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);  // Inisialisasi PWM dengan duty cycle 0

  pinMode(sw,INPUT_PULLUP);
  pinMode(hijau,OUTPUT);
  pinMode(fwd,OUTPUT);

  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE);
  initializing = false;

}

void loop() {
    lcd.setCursor(0,0);
    lcd.print("RPM : ");
    
    time_per_second = millis() - time_per_second_prev;
    
    if (time_per_second > 1000){
      pulse_per_second = counter;
      time_per_second_prev = millis();
      counter = 0;
      
    }
     
    if (digitalRead(sw) == HIGH) {
      digitalWrite(hijau, HIGH);
      int p1 = !digitalRead(prox1);
      int p2 = !digitalRead(prox2);

      if (initializing == false){
          if (pulse_per_second  > 0){
              direction_motor = true;
          }
          else{
              direction_motor = false;
          }
          p1_lock = false;
          p2_lock = false;
      } else {
        
        if (pulse_per_second  > 0){
          winch_direction = true;
        } else {
          winch_direction = false;
        }
        
      }

        
      if (p1 == HIGH && p1_lock == false){
          initializing = true;
          p1_lock = true;
          p2_lock = false;
          direction_motor = !direction_motor;   
      }
        
       if (p2 == HIGH && p2_lock == false){
           initializing = true;
           p1_lock = false;
           p2_lock = true;
           direction_motor = !direction_motor;
       }

       if (winch_direction != winch_direction_prev){
        p1_lock = !p1_lock;
        p2_lock = !p2_lock;
        direction_motor = !direction_motor;
       }
       
       if (direction_motor == true){
        lcd.setCursor(0,1);
        lcd.print("forward");
       } else {
        lcd.setCursor(0,1);
        lcd.print("reverse");
       }      
       }  
   
    else{
        digitalWrite(hijau, LOW);
        initializing = false;
        lcd.setCursor(0,1);
        lcd.print("STANDBY");
    }


    if (sw_prev != digitalRead(sw)){
      lcd.clear();
    }
    digitalWrite(fwd, direction_motor);


    winch_direction_prev = winch_direction;
    sw_prev = digitalRead(sw);
}


void updateEncoder(){
  
  currentStateCLK = digitalRead(CLK);
  if ((currentStateCLK != lastStateCLK)  && (currentStateCLK == 1)){
    if (digitalRead(DT) != currentStateCLK) {
      counter = counter - 1;
    } else {
      counter = counter + 1;
    }
  }
  
  lastStateCLK = currentStateCLK;
  
}
