#define POWER_PIN 14

#define PWM_PIN   26
#define PWM1_Ch   0
#define PWM1_Res  8
#define PWM1_Freq 1000
#define PUSH1     32
#define PUSH2     23
 
int PWM1_DutyCycle = 255;
byte power_state = 1;

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(PUSH1, INPUT_PULLUP);
  pinMode(PUSH2, INPUT_PULLUP);

  ledcAttachPin(PWM_PIN, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  digitalWrite(POWER_PIN, power_state);
  ledcWrite(PWM1_Ch, 128);
}

void loop() {
  int val1 = digitalRead(PUSH1);
  int val2 = digitalRead(PUSH2);
  
  if (!val1) 
    Serial.println("PUSH1");
  if (!val2) 
    Serial.println("PUSH2");
  
  delay(100);
}
