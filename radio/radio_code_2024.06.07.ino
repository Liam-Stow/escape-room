constexpr int SWITCH_PIN = 13;
constexpr int LED_RELAY_PIN = 3;
constexpr int PLAY_SOUND_PIN = 4;

void setup()
{
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_RELAY_PIN, OUTPUT);
  pinMode(PLAY_SOUND_PIN, OUTPUT);

  Serial.begin(115200);

  // Set outputs to default state
  digitalWrite(LED_RELAY_PIN, LOW);
  digitalWrite(PLAY_SOUND_PIN, HIGH); // sounds play when pin is set to LOW. Set to HIGH to begin with so nothing plays.
}

void loop()
{
  if (digitalRead(SWITCH_PIN) == LOW)     // <- Check the switch state
  {
    digitalWrite(PLAY_SOUND_PIN, LOW);    // <- Turn on the sound
    digitalWrite(LED_RELAY_PIN, HIGH);    // <- Turn on the LED
    Serial.println("Switch detected - playing sound");
  }
  else
  {
    digitalWrite(PLAY_SOUND_PIN, HIGH);   // <- Turn off the sound
    digitalWrite(LED_RELAY_PIN, LOW);     // <- Turn off the LED
    Serial.println("Switch not detected - not playing sound");
  }

  delay(100);
}
