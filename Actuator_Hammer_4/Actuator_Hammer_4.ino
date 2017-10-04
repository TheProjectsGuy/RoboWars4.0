/*
   Hammer With relays
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

#define ULTRASONIC_TX_PIN 2
#define ULTRASONIC_RX_PIN 3

#define ULTRASONIC1_THRESHOLD_VALUE_CM 17

#define HAMMER_INACTIVE_TIME_MILLIS 2000  //Time for which everything is stopped
#define HAMMER_ACTIVE_SMASH_TIME_MILLIS 550 //Hammer in air going towards ground
#define HAMMER_RETRIEVE_DELAY_PAUSE_TIME_MILLIS 750  //Hammer on ground

#define ENERGISE_RELAY_BY_LOGIC_LEVEL HIGH  //Turn on the relay by giving it a low (Active low - COM connects to NO), enter HIGH if using without the breakout board
#define ACTIVATE_RELAY_PIN 8  //COM - MOTOR, NC - GND, NO - 12V
#define RETRIEVE_RELAY_PIN 6  //COM - MOTOR, NC - GND, NO - 12V

#define LIMITER_SWITCH_PIN 5  //Use COM and NO

Ultrasonic_Sensor Ultrasonic1;

void setup() {
  // put your setup code here, to run once:
  Ultrasonic1.AttachToTrigEcho(ULTRASONIC_TX_PIN, ULTRASONIC_RX_PIN);
  Ultrasonic1.SetThresholdTo(ULTRASONIC1_THRESHOLD_VALUE_CM);

  pinMode(ULTRASONIC_TX_PIN, OUTPUT);
  pinMode(ULTRASONIC_RX_PIN, INPUT);
  pinMode(ACTIVATE_RELAY_PIN, OUTPUT);
  pinMode(RETRIEVE_RELAY_PIN, OUTPUT);
  pinMode(LIMITER_SWITCH_PIN, INPUT);

  Serial.begin(9600);
  //Keep retrieving
  Retrieve_Hammer();
}

void Retrieve_Hammer() {
  Serial.println("RETRIVEING HAMMER");
  while (digitalRead(LIMITER_SWITCH_PIN) == LOW) {
    digitalWrite(RETRIEVE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);    //Energise Retrieving hammer
    digitalWrite(ACTIVATE_RELAY_PIN, !ENERGISE_RELAY_BY_LOGIC_LEVEL);
  }
  //Hold the hammer there
  digitalWrite(RETRIEVE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);    //Energise Retrieving hammer
  digitalWrite(ACTIVATE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);

}

void loop() {
  // put your main code here, to run repeatedly:
  float Distance = Ultrasonic1.GetDistance();
  Serial.print("1 - ");
  Serial.print(Distance);
  Serial.print((Ultrasonic1.DidCrossThreshold()) ? " YES : " : " NO : ");
  Serial.println(Ultrasonic1.GetThresholdValue());

  if (Ultrasonic1.DidCrossThreshold()) {
    Serial.println("Hitting with hammer");
    digitalWrite(ACTIVATE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);  //SMASH - Energise the smashing relay
    digitalWrite(RETRIEVE_RELAY_PIN, !ENERGISE_RELAY_BY_LOGIC_LEVEL);
    delay(HAMMER_ACTIVE_SMASH_TIME_MILLIS);

    //Get the relays and the entire system to rest
    digitalWrite(ACTIVATE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);  
    digitalWrite(RETRIEVE_RELAY_PIN, ENERGISE_RELAY_BY_LOGIC_LEVEL);

    delay(HAMMER_RETRIEVE_DELAY_PAUSE_TIME_MILLIS);
    //Retrieve hammer and don't do anything for a desired time
    Retrieve_Hammer();
    delay(HAMMER_INACTIVE_TIME_MILLIS);
  }

  if (digitalRead(LIMITER_SWITCH_PIN) != HIGH) {  
    Retrieve_Hammer();
  } 
  
}
