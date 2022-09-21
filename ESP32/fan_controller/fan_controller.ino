#include <Button2.h>
#include <RotaryEncoder.h>

#define DEBUG             1
#define POWER_PIN1        12
#define POWER_PIN2        14
#define LEDC_CHANNEL_0    0
#define LEDC_CHANNEL_1    1
#define LEDC_TIMER_8_BIT  8
#define LEDC_BASE_FREQ    1000
#define FAN_PIN0          27
#define FAN_PIN1          26
#define DEF_PWM_VALUE     128
#define MIN_PWM_VALUE     0
#define MAX_PWM_VALUE     255

#define PUSH1             32
#define PUSH2             23
#define ROTARY1_PIN1      25
#define ROTARY1_PIN2      33
#define ROTARY2_PIN1      5
#define ROTARY2_PIN2      18
 
int PWM1_Duty = 255;
byte power_state = 1;

typedef struct {
  byte  number;
  int   power_pin;
  int   channel;
  byte  state;
  byte  pwm_value;
} Fan;

Button2 button_1, button_2;
RotaryEncoder encoder1(ROTARY1_PIN2, ROTARY1_PIN1, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoder2(ROTARY2_PIN2, ROTARY2_PIN1, RotaryEncoder::LatchMode::FOUR3);
Fan fan1 = { 1, POWER_PIN1, LEDC_CHANNEL_0, 0, DEF_PWM_VALUE };
Fan fan2 = { 2, POWER_PIN2, LEDC_CHANNEL_1, 0, DEF_PWM_VALUE };

void setup() {
  #if DEBUG
    Serial.begin(115200);
  #endif

  // power section
  pinMode(POWER_PIN1, OUTPUT);
  pinMode(POWER_PIN2, OUTPUT);

  // Buttons section
  button_1.begin(PUSH1);
  button_2.begin(PUSH2);
  button_1.setReleasedHandler(released);
  button_2.setReleasedHandler(released);

  // Encoders section
  encoder1.setPosition(DEF_PWM_VALUE);
  encoder2.setPosition(DEF_PWM_VALUE);

  // PWM section
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT);
  ledcAttachPin(FAN_PIN0, LEDC_CHANNEL_0);
  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT);
  ledcAttachPin(FAN_PIN1, LEDC_CHANNEL_1);
  ledcWrite(LEDC_CHANNEL_0, DEF_PWM_VALUE);
  ledcWrite(LEDC_CHANNEL_1, DEF_PWM_VALUE);
}

void loop() {
  button_1.loop();
  button_2.loop();
  
  check_state(&fan1);
  check_state(&fan2);

  check_rotary(&fan1, encoder1);
  check_rotary(&fan2, encoder2);
}

void released(Button2& btn) {
  if (btn == button_1) {
    _println("[ BUTTON 1 ] RELEASED");
    switch_state(&fan1);
  } else if (btn == button_2) {
    _println("[ BUTTON 2 ] RELEASED");
    switch_state(&fan2);
  }
}

void switch_state(Fan *fanny) {
  fanny->state = !(fanny->state);
  _print("[ FAN ");
  _print(String(fanny->number));
  _print(" ] NEW STATE:\t");
  _println(String(fanny->state));
}

void check_state(Fan *fanny) {
  if (fanny->state)
    digitalWrite(fanny->power_pin, HIGH);
  else
    digitalWrite(fanny->power_pin, LOW);
}

void set_pwm(Fan *fanny, byte value) {
  if (value != fanny->pwm_value) {
    fanny->pwm_value = value;
    ledcWrite(fanny->channel, value);
  }
}

void check_rotary(Fan *fanny, RotaryEncoder &enc) {
  enc.tick();
  int new_val = enc.getPosition(); // 256 (precedente 255)
  
  if (new_val != fanny->pwm_value) { // yes
    new_val = constrain(new_val, MIN_PWM_VALUE, MAX_PWM_VALUE); // 255
    enc.setPosition(new_val);
    set_pwm(fanny, new_val); // pwm = 255
    //enc.setPosition(new_val); // position = 255
    
    _print("[ FAN ");
    _print(String(fanny->number));
    _print(" ] ");
    _print("SET NEW PWM TO VALUE:\t");
    _println(String(fanny->pwm_value));
  } 
}

int _print(String s) {
  #if DEBUG
    Serial.print(s);
  #endif
  return 0;
}

int _println(String s) {
  #if DEBUG
    Serial.println(s);
  #endif
  return 0;
}
