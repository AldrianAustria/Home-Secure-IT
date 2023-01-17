// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID           "TEMPLATE ID"
#define BLYNK_DEVICE_NAME           "TEMPLATE NAME"
#define BLYNK_AUTH_TOKEN            "AUTHENTICATION TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "SSID";
char pass[] = "PASSWORD";

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27,16,2);

#define btn 14
#define pint 12
#define echo 13
#define trig 15
#define bzr 16

#define trig_thresh 10

int toggle = 1;
int event = 0;
int old_event = 1;

int photo_int = 1;

int duration;
int distance;
int comp_distance;
int presence = 0;

String status = String("DEACTIVATED");

BLYNK_WRITE(V0){
  toggle = param.asInt();
}

BLYNK_CONNECTED(){
  Blynk.virtualWrite(V6, "Online");
}

void alarm();
int ultra_sonic();

void setup(){
  lcd.init();
  lcd.backlight();
  pinMode(pint, INPUT);
  pinMode(btn, INPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(bzr, OUTPUT);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}

void loop(){
  lcd.setCursor(0,0);
  lcd.print("Alarm Status:");
  Blynk.virtualWrite(V0, toggle);
  Blynk.virtualWrite(V4, status);
  event = digitalRead(btn);
  if(event == HIGH && old_event == 0){
    if(toggle == 0){
      toggle = 1;
    }
    else {
      toggle = 0;
    }
  }
  old_event = event;
  if(toggle == 1){
    status = String("ACTIVATED");
    lcd.setCursor(0,1);    
    lcd.print(status);
    alarm();
  }
  else{
    status = String("DEACTIVATED");
    noTone(bzr);
    lcd.setCursor(0,1);    
    lcd.print(status);
    Blynk.virtualWrite(V7, "DEACTIVATED");
    Blynk.virtualWrite(V8, "DEACTIVATED");
    Blynk.virtualWrite(V2, 0);
  }
  delay(1);
  lcd.clear();
}

void alarm(){
  photo_int = digitalRead(pint);
  comp_distance = ultra_sonic();
  if (comp_distance <= trig_thresh) {
    presence = 1;
    Blynk.virtualWrite(V8, "POSITIVE");
    Blynk.virtualWrite(V2, presence);
  }
  else{
    presence = 0;
    Blynk.virtualWrite(V8, "NEGATIVE");
    Blynk.virtualWrite(V2, presence);
  }
  if (photo_int == 1) {
    Blynk.virtualWrite(V7, "OPENED");
    for(int i = 500; i<= 800; i++){
      tone(bzr, i);
      delay(1);
    }
    delay(50);
    for(int i = 800; i>= 500; i--){
      tone(bzr, i);
      delay(1);
    }
  }
  else{
    Blynk.virtualWrite(V7, "CLOSED");
    noTone(bzr);
  }
}

int ultra_sonic(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration*0.0343/2);
  return distance;
}
