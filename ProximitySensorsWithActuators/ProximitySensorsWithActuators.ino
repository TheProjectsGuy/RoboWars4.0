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
        return false;
      }
      else {
        return true;
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


Ultrasonic_Sensor Ultrasonic1,Ultrasonic2;

#define ULTRASONIC1_THRESHOLD_VALUE_CM 30
#define ULTRASONIC2_THRESHOLD_VALUE_CM 20

void setup() {
  //Pin declarations
  Ultrasonic1.AttachToTrigEcho(2,3);
  Ultrasonic1.SetThresholdTo(ULTRASONIC1_THRESHOLD_VALUE_CM);
  Ultrasonic2.AttachToTrigEcho(8,9);
  Ultrasonic2.SetThresholdTo(ULTRASONIC2_THRESHOLD_VALUE_CM);

  Serial.begin(9600);
}

void loop() {
  float Distance = Ultrasonic1.GetDistance();
  Serial.print("1 - ");
  Serial.print(Distance); 
  Serial.print((Ultrasonic1.DidCrossThreshold()) ? " NO : " :" YES : ");
  Serial.print(Ultrasonic1.GetThresholdValue());

  

  Serial.println();
}

