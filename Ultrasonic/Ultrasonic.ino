#define TRIG D5
#define ECHO D6

long duration;
float distance;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // Send 10ms pulse to trig pin
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // pulseIn outputs duration in microseconds
  duration = pulseIn(ECHO, HIGH);
  // Speed of sound = 343 m/s
  // distance in cm = ((duration / 10000000) * 343 * 100) / 2
  distance = duration * 0.01715;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);
}
