#include <EEPROM.h> //영구저장용 EEPROM
int dig[4]={16,16,16,16}; //화면에 표시되는 digit
int pin[8]={36,37,38,39,40,41,42,43}; //7-seg 핀
int com[4]={28,29,30,31}; //7-seg 그라운드 핀
int prevtime = 0; //마지막으로 도어가 열린 시간
int passwd[4]; //비밀번호 저장하는 변수
int digits[][8]= //숫자에 해당하는 7-seg
{
  {0,0,0,0,0,0,1,1},//0
  {1,0,0,1,1,1,1,1},//1
  {0,0,1,0,0,1,0,1},//2
  {0,0,0,0,1,1,0,1},//3
  {1,0,0,1,1,0,0,1},//4
  {0,1,0,0,1,0,0,1},//5
  {0,1,0,0,0,0,0,1},//6
  {0,0,0,1,1,1,1,1},//7
  {0,0,0,0,0,0,0,1},//8
  {0,0,0,0,1,0,0,1},//9
  {0,0,0,1,0,0,0,1},//A
  {1,1,0,0,0,0,0,1},//B
  {1,1,1,0,0,1,0,1},//C
  {1,0,0,0,0,1,0,1},//D
  {0,1,1,0,0,0,0,1},//E
  {0,1,1,1,0,0,0,1},//F
  {1,1,1,1,1,1,1,1}//NONE
};
void setup(){
  Serial.begin(9600);
  for (int i=36;i<44;i++){
    pinMode(i,OUTPUT);
  }//7-seg OUT
  for (int i=28;i<42;i++){
    pinMode(i,OUTPUT);
  }//7-seg GND
  for (int i=10;i<=13;i++){
    pinMode(i,INPUT_PULLUP);
  }//Button 4x4 INPUT
  for (int i=4;i<=7;i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,HIGH);
  }//Button 4x4 PWR
  pinMode(8,OUTPUT);//SERVO_MOTOR
  pinMode(2,OUTPUT);//SOUND
  analogWrite(8,40);//문 닫기
  /*
  EEPROM.write(0,1);
  EEPROM.write(1,2);
  EEPROM.write(2,3);
  EEPROM.write(3,0xF);*/ //EEPROM 덮어쓰기
  pwReadROM(); //EEPROM에서 비밀번호 읽어오기
}
int numbers[]={1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
//버튼에 할당한 값들

int last = 999; //마지막 눌린 버튼 (999는 나올 수 없는 값)
#define PW_CHANGE_1 3  //비밀번호 바꾸기 1단계
#define PW_CHANGE_2 4  //비밀번호 바꾸기 2단계
#define DOOR_OPEN 1  //문열림상태
#define NORMAL 0  //보통 상태
int mode = NORMAL; //기본 모드
long opentime = 0; //문이 열린 시간
void loop(){
  int lastlocal=999;
  // put your main code here, to run repeatedly:
  for(int row=4;row<=7;row++){
    digitalWrite(row,LOW);
    for(int col=10;col<=13;col++){
      if(isOn(col)){
        lastlocal=col;
        if(col != last){
          tone(2,392,30);
          int curnum=(row-4)*4+(col-10);
          Serial.println(numbers[curnum]);
          if(numbers[curnum]<10 && (!digitsFull()))setNum(curnum);
          else if(numbers[curnum]>0XD && (!digitsFull()))setNum(curnum);
          else if(numbers[curnum]==0xB)resetNum();
          else if(numbers[curnum]==0xA)delNum();
          else if(numbers[curnum]==0xD){
            if(mode == NORMAL)chkPwd();
            else if(mode == PW_CHANGE_1){
              setPwd();
              resetNum();
              tone(2,392);
              delay(125);
              tone(2,523);
              delay(125);
              noTone(2);
              mode = PW_CHANGE_2;
              }
            else if(mode == PW_CHANGE_2){
              if(isPwd()){
                mode=NORMAL;
                pwWriteROM();
                tone(2,392);
                delay(125);
                noTone(2);
                delay(125);
                tone(2,392);
                delay(125);
                tone(2,523);
                delay(125);
                noTone(2);
              }
              else {
                mode = PW_CHANGE_1;
                tone(2,392);
                delay(200);
                noTone(2);
                delay(10);
                tone(2,392);
                delay(200);
                noTone(2);
                }
              resetNum();
              }
            else chkPwd();
            }
          else if(numbers[curnum]==0xC
                  && digitsFull()
                  && isPwd()){
                    mode = PW_CHANGE_1;
                    resetNum();
                    }
        }
      }
      //delay(1);
      }
    for(int i=4;i<=7;i++)digitalWrite(i,HIGH);
    }
    last=lastlocal;
  //if(digitsFull())chkPwd();
  if(millis()-opentime > 6000 && mode == DOOR_OPEN){
    analogWrite(8,40);
    }
  if(millis()-opentime > 6200 && mode == DOOR_OPEN){
    digitalWrite(8,LOW);
    mode = NORMAL;
    }
  for (int i=0;i<4;i++){
    for (int j=0;j<4;j++){
      digitalWrite(com[j],HIGH);
    }
    for (int num=0;num<8;num++){
      digitalWrite(pin[num],!digits[dig[3-i]][num]);
    }
    digitalWrite(com[i],LOW);
    delay(1);
  }
  for (int j=0;j<4;j++){
    digitalWrite(com[j],HIGH);
  }
}

bool isOn(int pin){
  for(int i=0;i<3000;i++){
    if(digitalRead(pin))return false;
    }
    return true;
  }
int pwd= 0;
void setNum(int num){
  //if(num<10){}
  for(int i=2;i>=0;i--){dig[i+1]=dig[i];}
  dig[0]=numbers[num];
  }
void resetNum(){
  for(int i=0;i<4;i++)dig[i]=16;
  }
void delNum(){
  for(int i=0;i<3;i++)dig[i]=dig[i+1];
  dig[3]=16;
  }
bool digitsFull(){
  return dig[3]!=16;
  }
/*
void setNumDisp(int num){
  for(int i = 0; i<4;i++){
      dig[i] = num % 10;
      num /= 10;
    }
  }
  */
void setPwd(){
  for(int i=0;i<4;i++)passwd[i] = dig[3-i];
  }
bool isPwd(){
  bool checker=true;
  for(int i=0;i<4;i++)checker &= (passwd[i] == dig[3-i]);
  return checker;
}
void chkPwd(){
  if(isPwd()){
    resetNum();
    Serial.println("Yes");
    analogWrite(8,160);
    mode = DOOR_OPEN;
    opentime = millis();
    //SOUND
    tone(2,262);
    delay(250);
    digitalWrite(8,LOW);
    tone(2,330);
    delay(250);
    tone(2,392);
    delay(250);
    noTone(2);
  }
  else {
    resetNum();
    Serial.println("No");
    //SOUND
    tone(2,392);
    delay(200);
    noTone(2);
    delay(10);
    tone(2,392);
    delay(200);
    noTone(2);
  }
  }
void pwReadROM(){
  for(int i=0;i<4;i++)passwd[i]=EEPROM.read(i);
  }
void pwWriteROM(){
  for(int i=0;i<4;i++)EEPROM.write(i,passwd[i]);
  }
