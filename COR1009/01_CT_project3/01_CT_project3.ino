const int EN = 9;
const int MC1 = 3;
const int MC2 = 2;
const int T_Sensor = A1;
#define POWER 255
#define STR 180
#define WEAK 90
#define HOT 35.0
#define STT 10
int previous = 0;
void forwad(int rate);
void reverse(int rate);
void brake();



void setup(){
  pinMode(EN,OUTPUT);
  pinMode(MC1,OUTPUT);
  pinMode(MC2,OUTPUT);
  pinMode(T_Sensor,INPUT);
  for(int i=4;i<=12;i++)pinMode(i,OUTPUT);
  brake();
}


void loop(){
  if(check_temp() - 0.5 > HOT){
    change_mode(3);
    while(check_temp() + 1.0 > HOT);
    change_mode(previous);
    } // 파워 모드
  else{
    int input = read_sw();
    if (input != -1) {
      delay(50);
      change_mode(input);
    } // 일반 모드
    }
}

float check_temp() {
  float meanvolt = 0;
  for(int i=0;i<100;i++)meanvolt+=analogRead(T_Sensor)*0.5/1024;
  meanvolt = meanvolt/10;
  float celsiustemp=(meanvolt-0.5)*100;
  return celsiustemp;
} // 온도 읽기 - 100번 읽어 평균을 내서 정확한 값을 도출한다

int read_sw(){
  for(int i = STT;i<=STT+2;i++){
    if(digitalRead(i)){
      return i-STT;
    }
    }
    return -1;
  } //스위치 값을 읽어 번호를 돌려준다. 없으면 -1

int modes[] = {0,WEAK,STR,POWER};
void change_mode(int mode){
  change_LED(mode);
  if(mode){
    if(mode<3)previous=mode;
    make_sound(mode);
    forward(modes[mode]);
    }
  else brake();
  delay(10);
  } //모드 변경
int freq[] = {400,500,600,900};
void make_sound(int mode){
  for(int i=4;i<=6;i++){
    if((i-mode) == 3){
      tone(7,freq[mode],30);
      delay(30);}
    }
  } //소리 내기
void change_LED(int mode){
  for(int i=4;i<=6;i++){
    if((i-mode) == 3){
      digitalWrite(i,HIGH);}
    else digitalWrite(i,LOW);
    }
  } // LED 색 바꾸기

void forward(int rate)
{
  digitalWrite(EN,HIGH);
  digitalWrite(MC1,HIGH);
  digitalWrite(MC2,LOW);
  analogWrite(EN, rate);
} //전진
void brake()
{
  digitalWrite(EN,LOW);
  digitalWrite(MC1,LOW);
  digitalWrite(MC2,LOW);
  analogWrite(EN, HIGH);
} //멈추기

