int adcValue = 0;

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);
}

void loop() {
  // read the analog in value
  adcValue = analogRead(A0);
 
  // print the readings in the Serial Monitor
  Serial.println(adcValue);
  
  delay(1000);
}