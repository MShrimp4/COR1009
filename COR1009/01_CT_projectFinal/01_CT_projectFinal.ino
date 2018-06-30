#include <LiquidCrystal_I2C.h>
#define SOUND 8//소리 포트
LiquidCrystal_I2C lcd(0x3F,20,4);
#define EN 6
#define MC1 3
#define MC2 2
#define trig 39
#define echo 37
typedef struct Ctrl {
  void (*Updater)(float);
  void (*Stopper)();
  } Control;
Control c[] = {{LCDUpdater,LCDStopper},{MTRUpdater,MTRStopper},{SNDUpdater,SNDStopper}}; //업데이트 함수
int in[] = {35,echo};
int out[] = {trig,41,MC1,MC2,EN};
void setup()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
  for(int i=0;i<(sizeof(in)/sizeof(in[0]));i++)pinMode(in[i],INPUT);
  for(int i=0;i<(sizeof(out)/sizeof(out[0]));i++)pinMode(out[i],OUTPUT);
  digitalWrite(35,LOW);
  digitalWrite(41,HIGH);
  brake();
  Serial.begin(9600);
}
void loop(){
  float length = ReadLength();//거리 측정
  for(int i=0;i<(sizeof(c)/sizeof(c[0]));i++)DoOnLength(c[i],length);//거리에 맞게 업데이트
  delay(500);//500ms 기다리기
}
void DoOnLength(Control c, float len){ //업데이트 관리 함수
  if(len<5)c.Stopper();
  else if(len<129)c.Updater(len);
}
void LCDUpdater(float len){ // LCD 거리 업데이트
  char LCDMSG[8];
  dtostrf(len,3,1,LCDMSG);
  lcd.clear();
  lcd.print("Distance: ");
  lcd.print(LCDMSG);
}
void LCDStopper(){ //멈춤 시 LCD 업데이트
  lcd.clear();
  lcd.print("      STOP      ");
}
void SNDUpdater(float len){tone(SOUND,len<100?map(len,5,100,2000,50):50,50);} // 거리에 따른 소리 업데이트
void SNDStopper(){tone(SOUND,3000);} // 멈춤 시 삐 소리
void MTRUpdater(float len){forward(map(len,5,80,80,240));} // 거리에 따른 모터 거리 업데이트
void MTRStopper(){brake();} //멈춤 시 모터 멈춤
float ReadLength(){ // 거리 재기
  delay(10);
  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long duration = pulseIn(echo, HIGH); 
  if(duration == 0)return 80; // Division by 0 피하기
  float distance = (((float)duration / 10000.0)*340.0 / 2);
  if(distance > 80)return 80; //거리 자르기
  return distance;
} 
int previous = 0;
void forward(int rate) { //전진
  if(abs(previous-rate)>15){ //너무 많은 폴링을 저지하기 위해 큰 폭으로 속도가 바뀌어야 명령이 들어감
    digitalWrite(EN,LOW);
    digitalWrite(MC1,HIGH);
    digitalWrite(MC2,LOW);
    analogWrite(EN, rate);
    Serial.println(rate);
    }
  previous = rate;
}
void brake(){ //멈추기
  digitalWrite(EN,LOW);
  digitalWrite(MC1,LOW);
  digitalWrite(MC2,LOW);
  analogWrite(EN, HIGH);
  previous = 0;
}

