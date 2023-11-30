#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define prox1 3 //kanan searah jarum jam
#define prox2 4 //kiri  kebalikan
#define sw 10
#define hijau 8
#define fwd 9
#define off 7
const int pwmPin = 11;  // Pin PWM pada Arduino Uno
const int rpmTarget = 1000;  // Kecepatan target dalam RPM

const int sensorPin = 2;  // Pin digital yang terhubung ke output sensor
volatile unsigned long count = 0;  // Variabel untuk menyimpan jumlah putaran
const float jarakAntarGigi = 2.0;  // Jarak antara setiap gigi dalam sentimeter

void setup() {

  lcd.init();
  lcd.backlight();
  pinMode(prox1,INPUT);
  pinMode(prox2,INPUT);
  pinMode(off,INPUT);

  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);  // Inisialisasi PWM dengan duty cycle 0

  pinMode(sw,INPUT);
  pinMode(hijau,OUTPUT);
  pinMode(fwd,OUTPUT);

  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);
  //Serial.begin(9600);



}

void loop() {


        while (true) {

        if (digitalRead(sw) == HIGH) {
            digitalWrite(hijau, HIGH);
            kondisi();
            //delay (200);
        }  
        
        if (digitalRead(off) == HIGH){
            digitalWrite(hijau, LOW);
            lcd.clear();
        }
        
    }

}

void countPulse() {
    // Fungsi ini akan dipanggil setiap kali sensor mendeteksi putaran
    count++;
  }

void setMotorSpeed(int targetRPM) {
  // Konversi RPM ke nilai PWM
  int pwmValue = map(targetRPM, 0, 3000, 0, 255);  // Sesuaikan dengan karakteristik motor dan PWM
  
  // Atur PWM
  analogWrite(pwmPin, pwmValue);
  
  // Tampilkan informasi ke Serial Monitor
  //Serial.print("Target RPM: ");
  //Serial.print(targetRPM);
  //Serial.print(", PWM Value: ");
 // Serial.println(pwmValue);
}

  void kondisi(){
    detachInterrupt(digitalPinToInterrupt(sensorPin));  // Matikan interrupt sementara
    unsigned long pulseCount = count;  // Baca jumlah putaran
    count = 0;  // Reset jumlah putaran
    attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);  // Aktifkan interrupt kembali

    // Hitung kecepatan dalam sentimeter per detik
    float jarakPerputaran = jarakAntarGigi * 114;  // Total jarak dalam sentimeter per putaran
    float speed = (pulseCount * jarakPerputaran) / 1.0;  // Kecepatan dalam sentimeter per detik
    lcd.setCursor(0,0);
    lcd.print("Speed:");
    lcd.print(speed);
    lcd.setCursor(11,0);
    lcd.print("Rpm");

    //lcd.clear();
    delay(200);


    int p1 = digitalRead(prox1);
    int p2 = digitalRead(prox2);

    if (p1 == HIGH && p2 == LOW ){
    lcd.setCursor(0,1);
    lcd.print("Forward");
    digitalWrite(fwd, LOW);
    setMotorSpeed(rpmTarget);
    delay(2000);
    //setMotorSpeed(0);
    }else {
    lcd.setCursor(0,1);
    lcd.print("                   ");
    }
    
    delay(200);

    if (p1 == LOW && p2 == HIGH ){
    lcd.setCursor(0,1);
    lcd.print("Reverse");
    digitalWrite(fwd, HIGH);
    setMotorSpeed(rpmTarget);
    delay(2000);
    //setMotorSpeed(0);
    }else {
    lcd.setCursor(0,1);
    lcd.print("                   ");

    }
    delay(200);

    

}
