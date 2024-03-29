#define INT_PIN D2

IRAM_ATTR void interruptHandler(){
  Serial.println("Button Pressed!");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(INT_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), interruptHandler, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("hello");
  delay(1000);

}
