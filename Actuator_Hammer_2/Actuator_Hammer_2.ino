/*
   CODE FOR L298 Control....
   Connect the 300RPM MOTOR to the driver.
*/

class Ultrasonic_Sensor {
    int TRIG_PIN;
    int ECHO_PIN;

    float THRESHOLD_VALUE;

    float DISTANCE;
  public:
    void AttachToTrigEcho(int Trigger, int Echo) {
      TRIG_PIN = Trigger;
      ECHO_PIN = Echo;
      pinMode(TRIG_PIN, OUTPUT);
      pinMode(ECHO_PIN, INPUT);
    }

    bool Did_Cross_Threshold;
    void SetThresholdTo(float threshold_value) {
      THRESHOLD_VALUE = threshold_value;
    }
    float GetThresholdValue() {
      return THRESHOLD_VALUE;
    }
    bool DidCrossThreshold() {  //Came closer than the threshold value
      float currentDistance = this->GetDistance();
      if (currentDistance <= THRESHOLD_VALUE) {
        return true;  //TRIGGERED
      }
      else {
        return false; //NOT TRIGGERED
      }
    }

    void InitiateTrigger() {
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
    }
    float GetDuration() {
      InitiateTrigger();
      float Duration = pulseIn(ECHO_PIN, HIGH); //Get duration of the high pulse
      return Duration;
    }
    float GetDistance() {
      DISTANCE =  this->GetDuration() * 0.0343 / 2;
      return DISTANCE;
    }
};


Ultrasonic_Sensor Ultrasonic1;
//Ultrasonic Sensor
#define TX_ULTRASONIC 2
#define RX_ULTRASONIC 3
//Ultrasonic Threshold
#define ULTRASONIC1_THRESHOLD_VALUE_CM 20

//L298 pins
//IN1 pin
#define HIT_DIR_PIN 8
//IN2 pin
#define RETRIEVE_DIR_PIN 9
//PWM pin - EN1
#define EN_HAMMER_PIN 11

//The SPDT limiter switch. Connect the NO to a digital pin of DPDT (PULL DOWN)
#define HAMMER_AT_HOLD_PIN 10
//8 bit hammer speed
#define HAMMER_SPEED 255
//Hammer drop time
#define HAMMER_DROP_TIME 900

void setup() {
  Ultrasonic1.AttachToTrigEcho(TX_ULTRASONIC, RX_ULTRASONIC);
  Ultrasonic1.SetThresholdTo(ULTRASONIC1_THRESHOLD_VALUE_CM);

  pinMode(HIT_DIR_PIN, OUTPUT);
  pinMode(RETRIEVE_DIR_PIN, OUTPUT);
  pinMode(EN_HAMMER_PIN, OUTPUT);
  analogWrite(EN_HAMMER_PIN, HAMMER_SPEED);
  pinMode(HAMMER_AT_HOLD_PIN, INPUT);

  Serial.begin(9600);
  //Keep retrieving
  Retrieve_Hammer();
}

void Retrieve_Hammer() {
  Serial.println("RETRIEVING");
  while (digitalRead(HAMMER_AT_HOLD_PIN) == LOW) {
    //Retrieve motor to reset
    digitalWrite(RETRIEVE_DIR_PIN, HIGH);
    digitalWrite(HIT_DIR_PIN, LOW);
  }
  //STOP EVERYTHING
  Serial.println("RETRIEVING DONE");
  digitalWrite(RETRIEVE_DIR_PIN, LOW);
  digitalWrite(HIT_DIR_PIN, LOW);
}

void loop() {
  float Distance = Ultrasonic1.GetDistance();
  Serial.print("1 - ");
  Serial.print(Distance);
  Serial.print((Ultrasonic1.DidCrossThreshold()) ? " YES : " : " NO : ");
  Serial.println(Ultrasonic1.GetThresholdValue());

  if (Ultrasonic1.DidCrossThreshold()) {
    //SMASH
    Serial.println("Hitting with hammer");
    digitalWrite(RETRIEVE_DIR_PIN, LOW);
    digitalWrite(HIT_DIR_PIN, HIGH);  //HIT THE BOT
    delay(HAMMER_DROP_TIME);  //Time for the process to take place
    Retrieve_Hammer();
    //Wait
    delay(2000);
  }

  if (digitalRead(HAMMER_AT_HOLD_PIN) != HIGH) {
    Retrieve_Hammer();
  }
}

