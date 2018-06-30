#define T_Sensor A12
int counter;
int dig[4]={1,5,3,1}; //세그먼트에 표시할 것
int pin[8]={36,37,38,39,40,41,42,43};// VIN 역할 핀
int com[4]={28,29,30,31};//Common GND 역할 핀
int prevtime = 0; //LED 표시한 마지막 시간
int digits[10][8]= //숫자
{
  {0,0,0,0,0,0,1,1},
  {1,0,0,1,1,1,1,1},
  {0,0,1,0,0,1,0,1},
  {0,0,0,0,1,1,0,1},
  {1,0,0,1,1,0,0,1},
  {0,1,0,0,1,0,0,1},
  {0,1,0,0,0,0,0,1},
  {0,0,0,1,1,1,1,1},
  {0,0,0,0,0,0,0,1},
  {0,0,0,0,1,0,0,1}
};
void setup(){
  Serial.begin(9600);
  for (int i=36;i<44;i++){
    pinMode(i,OUTPUT);
  }
  for (int i=28;i<42;i++){
    pinMode(i,OUTPUT);
  }
  setTemp();
}
void loop(){
  if((millis() / 100) - prevtime > 6) { //약 0.6초마다 온도 체크
    setTemp();
    prevtime = millis() / 100;
    }
  for (int i=0;i<4;i++){ //세그먼트 표시
    for (int j=0;j<4;j++){
      digitalWrite(com[j],HIGH);
    }
    for (int num=0;num<8;num++){
      digitalWrite(pin[num],!digits[dig[3-i]][num]);
    }
    if(i == 1)digitalWrite(pin[7],HIGH);
    digitalWrite(com[i],LOW);
    delay(1);
  }
}

void setTemp(){
  counter = 100*check_temp();
  for(int i = 0; i<4;i++){
      dig[i] = counter % 10;
      counter /= 10;
    }
  } //온도를 읽어 세그먼트에 들어갈 숫자로 바꾼다. 0~99.99도까지만 가능함.
#define SUM 100
float prev_temp = 0;
float check_temp() {
  float meanvolt = 0;
  for(int i=0;i<SUM;i++)meanvolt+=analogRead(T_Sensor)*0.5/1024;
  meanvolt = meanvolt/SUM*10;
  float celsiustemp=(meanvolt-0.5)*100;
  Serial.println(celsiustemp);
  if(abs(prev_temp-celsiustemp)>0.05)prev_temp=celsiustemp;
  else celsiustemp=prev_temp;
  return celsiustemp;
} // 온도 읽기 - 100번 읽어 평균을 내서 정확한 값을 도출한다

