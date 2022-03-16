#include <SimpleKalmanFilter.h>
SimpleKalmanFilter  bo_loc(2, 2, 0.01);
#include <ArduinoJson.h>
StaticJsonDocument<200> CONFIG;

int pin = 2;//chan cam bien
volatile unsigned int pulse;
volatile unsigned int pulseTong = 0, pulse1;
int Relay = 8;
int x=0;//x: lượng nước yêu cầu (ml)
int t=0;//t: thời gian đếm xung (ml)
String message;

void setup() {
  Serial.begin(9600);
  pinMode(pin, INPUT);
  
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, LOW);
  delay(100);
}
void count_pulse() {
  pulse++;
}

void loop() {
  message = Serial.readString();
  delay(200);
  deserializeJson(CONFIG, message);
  
  x = CONFIG["x"];Serial.println("x: "+String(x));
  t = CONFIG["t"];Serial.println("t: "+String(t));
  delay(200);
  volatile unsigned int y = x * 6;Serial.println("y: "+String(y));
  //y = x*98*60/1000: số xung tương ứng lượng nước

  if (x != 0) {
    digitalWrite(Relay, HIGH);//bật máy bơm
    delay(100);
    for (int i = 0; i < 700; i++) {
      pulse = 0;
      attachInterrupt(0, count_pulse, FALLING);//bắt đầu đếm xung
      delay(t);
      detachInterrupt(0);//dừng đếm xung
      pulse1 = bo_loc.updateEstimate(pulse);

      Serial.print(pulse);Serial.print('+');
      Serial.println(pulse1);

      pulseTong = pulseTong + pulse1;
      if (pulseTong > y) {
        digitalWrite(Relay, LOW);//tắt máy bơm
        delay(100);
        break;}}}
  x=0; pulse = 0; pulseTong = 0;
}
