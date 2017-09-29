/*
 * Hammer with RELAY control
 * Make a Dual voltage supply (Use 2 batteries and connect them in series, next ground the connecting point)
 *  Connect one pin of motor to ground, other pin to COM of a relay
 *  From -12V, connect the COM of SPDT limiter switch
 *  From NC of SPDT limiter switch, connect to the NC of relay
 *  Connect the NO of relay to +12V (battery)
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

#define ULTRASONIC1_THRESHOLD_VALUE_CM 20
#define HAMMER_INACTIVE_TIME_MILLIS 2000

#define RELAY_PIN_1 8

void setup() {
  //Pin declarations
  Ultrasonic1.AttachToTrigEcho(2, 3);
  Ultrasonic1.SetThresholdTo(ULTRASONIC1_THRESHOLD_VALUE_CM);

  Serial.begin(9600);
  pinMode(RELAY_PIN_1, OUTPUT); //Relay connected
  digitalWrite(RELAY_PIN_1, HIGH);  //This will keep the hammer in reversing mode
}

/*
   1 - Hammer at rest - The SPDT switch will give OPEN (NC terminal)
   2 - Hammer down (When DidCrossThreshold) - This will give a LOW to the 12V to motor relay - Making the hammer hit the floor.....
   3 - After a delay of 2 seconds, Relay is given HIGH (Connected to reversing battery) and the SPDT switch will give CLOSED until the hammer reaches it's initial value
*/

void loop() {
  float Distance = Ultrasonic1.GetDistance();
  Serial.print("1 - ");
  Serial.print(Distance);
  Serial.print((Ultrasonic1.DidCrossThreshold()) ? " YES : " : " NO : ");
  Serial.print(Ultrasonic1.GetThresholdValue());

  if (Ultrasonic1.DidCrossThreshold() ) {
    //Drop hammer
    digitalWrite(RELAY_PIN_1, LOW); //Hammer starts dropping
    delay(1700);  //Hammer to strike the ground
    digitalWrite(RELAY_PIN_1, HIGH);  //Hammer retriving mode. NOTE that the SPDT will output HIGH (NO and COM pin): Hammer starts retriving
    //NOTE : It will automatically stop when it reaches initial point, SPDT will output LOW and disconnect the circuit

    delay(HAMMER_INACTIVE_TIME_MILLIS);  //We want nothing to work for the next five seconds

  }
  delay(5);


  Serial.println();
}

