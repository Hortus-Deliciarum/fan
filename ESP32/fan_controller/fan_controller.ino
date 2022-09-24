#include <Button2.h>
#include <HortusRotary.h>
#include <HortusWifi.h>

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
#define STEPVALUE         100

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
HortusRotary encoder1(ROTARY1_PIN2, ROTARY1_PIN1, HortusRotary::LatchMode::FOUR3);
HortusRotary encoder2(ROTARY2_PIN2, ROTARY2_PIN1, HortusRotary::LatchMode::FOUR3);
Fan fan1 = { 1, POWER_PIN1, LEDC_CHANNEL_0, 0, DEF_PWM_VALUE };
Fan fan2 = { 2, POWER_PIN2, LEDC_CHANNEL_1, 0, DEF_PWM_VALUE };

String FAN_1_STATE = "/fan/1/state";
String FAN_2_STATE = "/fan/2/state";
String FAN_1_SPEED = "/fan/1/speed";
String FAN_2_SPEED = "/fan/2/speed";
String AWAKE = "/fan/awake";

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

  // NETWORK Section

  HortusWifi(HortusWifi::Connection::HORTUS, 20, AWAKE);

  OscWiFi.subscribe(HortusWifi::RECV_PORT, FAN_1_STATE,
        [](const OscMessage& m) {
            float _state = m.arg<float>(0);
            set_fan_state(&fan1, _state);
            
            _print("[ OSC RECEIVED ] ");
            _println(FAN_1_STATE);
            
            send_osc((String&)m.address(), _state);
        });

    OscWiFi.subscribe(HortusWifi::RECV_PORT, FAN_1_SPEED,
        [](const OscMessage& m) {
            float _speed = m.arg<float>(0);
            set_fan_speed(encoder1, _speed);

            _print("[ OSC RECEIVED ] ");
            _println(FAN_1_SPEED);
            
        });

    OscWiFi.subscribe(HortusWifi::RECV_PORT, FAN_2_STATE,
        [](const OscMessage& m) {
            float _state = m.arg<float>(0);
            set_fan_state(&fan2, _state);

            _print("[ OSC RECEIVED ] ");
            _println(FAN_2_STATE);
            
            send_osc((String&)m.address(), _state);
        });

    OscWiFi.subscribe(HortusWifi::RECV_PORT, FAN_2_SPEED,
        [](const OscMessage& m) {
            float _speed = m.arg<float>(0);
            set_fan_speed(encoder2, _speed);

            _print("[ OSC RECEIVED ] ");
            _println(FAN_2_SPEED);
        });
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

void set_fan_state(Fan *fanny, float value)
{
    _print("[ SET ] FAN ");
    _print(String(fanny->number));
    _print(" STATE: ");
    
    if (value) {
        fanny->state = true;
        _println(String(1));
    }
    else {
        fanny->state = false;
        _println(String(0));
    } 
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

void set_fan_speed(HortusRotary& _enc, float _spd)
{
    int _speed = (int)_spd / STEPVALUE * STEPVALUE;
    _speed = constrain(_speed, MIN_PWM_VALUE, MAX_PWM_VALUE);
    _enc.setPosition(_speed);
}

void check_rotary(Fan *fanny, HortusRotary &enc) {
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

    if (fanny->number == 1)
        send_osc(FAN_1_SPEED, fanny->pwm_value);
    else if (fanny->number == 2)
        send_osc(FAN_2_SPEED, fanny->pwm_value);
  } 
}

void send_osc(String& addr, int value) 
{
    OscWiFi.send(HortusWifi::HOST, HortusWifi::SEND_PORT, addr, value);
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
