#define POWER_PIN 4

#define PWM_PIN   16
#define PWM1_Ch   0
#define PWM1_Res  8
#define PWM1_Freq 1000
 
int PWM1_DutyCycle = 255;
byte power_state = 1;

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);

  ledcAttachPin(PWM_PIN, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  digitalWrite(POWER_PIN, power_state);
  ledcWrite(PWM1_Ch, 255);
}

void loop() {
  Serial.println(255);
  ledcWrite(PWM1_Ch, 255);
  delay(5000);

  Serial.println(200);
  ledcWrite(PWM1_Ch, 200);
  delay(5000);

  Serial.println(160);
  ledcWrite(PWM1_Ch, 160);
  delay(5000);

  Serial.println(128);
  ledcWrite(PWM1_Ch, 128);
  delay(5000);

  Serial.println(96);
  ledcWrite(PWM1_Ch, 96);
  delay(5000);

  Serial.println(64);
  ledcWrite(PWM1_Ch, 64);
  delay(5000);

  Serial.println(32);
  ledcWrite(PWM1_Ch, 32);
  delay(5000);

  Serial.println(16);
  ledcWrite(PWM1_Ch, 16);
  delay(5000);

  Serial.println(8);
  ledcWrite(PWM1_Ch, 8);
  delay(5000);
}
