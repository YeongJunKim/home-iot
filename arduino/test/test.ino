void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

long nowTick;
long pastTick;
void loop() {
  // put your main code here, to run repeatedly:
  nowTick = millis();

  if(nowTick - pastTick  > 1000)
  {
  Serial.println("hello");
  pastTick = nowTick;    
  }
}
