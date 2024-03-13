#include <LiquidCrystal_I2C.h>
#include <Servo.h> 
#include <SoftwareSerial.h>

#define IR1 4
#define IR2 6
#define PIN_SERVO 5
#define LED_VERDE 10
#define LED_ROSU 9

LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;
SoftwareSerial mySerial(3, 2);

volatile int slot = 4;
volatile bool senzor1 = false;
volatile bool senzor2 = false;
bool flag1 = false;
bool flag2 = false;

String s = "";
String vectorNumere[5] = {
  "",
  "",
  "",
  "",
  ""
};
int count = 0;
String master = "0755973939";
bool ok = false;
bool mesajNou = false;
String telefon = "";
String mesaj = "";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); 
  updateSerial();
  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");
  updateSerial();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("FRINCU IOAN");
  lcd.setCursor(0,1);
  lcd.print("  Parcare"); 
  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROSU, OUTPUT);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_ROSU, LOW);

  myservo.attach(PIN_SERVO);
  myservo.write(90);

  pinMode(IR1, INPUT_PULLUP);
  pinMode(IR2, INPUT_PULLUP);
  delay(2000);
  lcd.clear();
}

void loop() {
  updateSerial();
  if(mesajNou){
    verificaMesaj();
  }
  bariera();
  afisareLCD();
  leduri();
  delay(1000);
}

void afisareLCD(){
  if(slot > 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Buna ziua!");
    lcd.setCursor(0,1);
    lcd.print("Locuri libere:");
    lcd.print(slot); 
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Parcare plina!");
  }
}

void leduri(){
  if(slot > 0){
    digitalWrite(LED_ROSU, LOW);
    digitalWrite(LED_VERDE, HIGH);
  }else{
    digitalWrite(LED_ROSU, HIGH);
    digitalWrite(LED_VERDE, LOW);
  }
}

void bariera() {
  if(digitalRead(IR1) == LOW && !flag1) {
     if(slot > 0) {
        flag1 = true;
        if(!flag2) {
           myservo.write(0);
        }
        delay(1000);
        while(flag1 && !flag2){
          if(digitalRead(IR2) == LOW){
            flag2 = true;
          }else{
            if(digitalRead(IR1) == LOW){
              flag1 = false;
            }
          } 
        }
        if(flag1 && flag2){
          slot = slot - 1;
        }
        flag1 = false;
        flag2 = false;
        delay(1000);
        myservo.write(90);
     }
  }
  
  if(digitalRead(IR2) == LOW && !flag2) {
        flag2 = true;
        if(!flag1) {
           myservo.write(0);
        }
        delay(1000);
        while(flag2 && !flag1){
          if(digitalRead(IR1) == LOW){
            flag1 = true;
          }else{
            if(digitalRead(IR2) == LOW){
              flag2 = false;
            }
          }
        }
        if(flag1 && flag2){
          slot = slot + 1;
        }
        flag1 = false;
        flag2 = false;
        delay(1000);
        myservo.write(90);
        
  }
}

void updateSerial()
{
  ok = false;
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) 
  {
    if(ok == false){
      s = "";
      ok = true;
      mesajNou = true;
    }
    char c = mySerial.read();
    s = s + c;
    Serial.write(c);
  }
  int len = s.length();
  mesaj = s.substring(50, len - 2); 
  telefon = s.substring(11, 11 + 10);
}

void verificaMesaj(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(mesaj);
  lcd.setCursor(0,1);
  lcd.print(telefon);
  delay(1000);
  
  if(mesaj == "Deschide"){
    bool verif = false;
    if(telefon == master){
      verif = true;
    }else{
      for(int i=0;i<count;i++){
        if(vectorNumere[i] == telefon){
          verif = true;
          break;
        }
      }
    }
    if(verif){
      myservo.write(0);
    }else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nu ai dreptul");
      lcd.setCursor(0,1);
      lcd.print("sa deschizi!");
    }
  }
  if(mesaj[0] == '0' && mesaj[1] == '7'){
    if(telefon == master){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("tel:");
      lcd.print(mesaj);
      lcd.setCursor(0,1);
      lcd.print("adaugat");
      vectorNumere[count++] = mesaj;
    }else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nu se poate");
      lcd.setCursor(0,1);
      lcd.print("adauga!!");
    }
  }
  delay(5000);
  mesajNou = false;
  myservo.write(90);
}
