constexpr int BUTTON_PIN = 13;
constexpr int LED_RELAY_PIN = 3;
constexpr int PLAY_SOUND_PIN = 4;

void setup() {
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(LED_RELAY_PIN, OUTPUT);
pinMode(PLAY_SOUND_PIN, OUTPUT);

Serial.begin(115200);
}

void loop() {
  digitalWrite(LED_RELAY_PIN, HIGH);
  digitalWrite(PLAY_SOUND_PIN, HIGH);

  Serial.println("Waiting for button press");
  while (digitalRead(BUTTON_PIN) == HIGH){}

  Serial.print("Button pressed");
  digitalWrite(LED_RELAY_PIN, LOW); //Turn LEDS on

  Serial.println("Playing sound");
  digitalWrite(PLAY_SOUND_PIN, LOW); //play sound
  delay(100);
  digitalWrite(PLAY_SOUND_PIN, HIGH);

  delay(10000);

  //Restart
  Serial.println("Sequence finished, waiting 2 minutes");
  delay(120000);
  Serial.println("----RESTARTING----");
}
