#define LED_PIN D2

int fade = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 255; i = (i + fade) % 256) {
    analogWrite(LED_PIN, i);
    delay(50);
  }
}
