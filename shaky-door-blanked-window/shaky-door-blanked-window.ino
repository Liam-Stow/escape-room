constexpr int BUTTON_PIN = 2;
constexpr int SCREEN_RELAY_PIN = 3;
constexpr int DOOR_SHAKER_REPLAY_PIN = 4;
constexpr int PLAY_SOUND_1_PIN = 7;
constexpr int PLAY_SOUND_2_PIN = 6;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SCREEN_RELAY_PIN, OUTPUT);
  pinMode(DOOR_SHAKER_REPLAY_PIN, OUTPUT);
  pinMode(PLAY_SOUND_1_PIN, OUTPUT);
  pinMode(PLAY_SOUND_2_PIN, OUTPUT);

	Serial.begin(115200);
}

void loop() {
  // Set outputs to default state
  digitalWrite(SCREEN_RELAY_PIN, LOW);
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);
  digitalWrite(PLAY_SOUND_1_PIN, HIGH); // sounds play when pin is set to LOW. Set to HIGH to begin with so nothing plays.
  digitalWrite(PLAY_SOUND_2_PIN, HIGH);

  // Wait for button press
  Serial.println("Waiting for button press");
  while (digitalRead(BUTTON_PIN) == LOW) {}
  Serial.println("Detected button press");

  // Play sound 1
  Serial.println("Playing sound 1");
  digitalWrite(PLAY_SOUND_1_PIN, LOW);
  delay(100);
  digitalWrite(PLAY_SOUND_1_PIN, HIGH);

  // Wait 10 seconds
  Serial.println("Waiting 10 seconds");
  delay(10000);

  // Drop screen
  Serial.println("Dropping screen");
  digitalWrite(SCREEN_RELAY_PIN, HIGH);

  // Play sound 2
  Serial.println("Playing sound 2");
  digitalWrite(PLAY_SOUND_2_PIN, LOW);
  delay(100);
  digitalWrite(PLAY_SOUND_2_PIN, HIGH);

  // Wait 7 seconds
  Serial.println("Waiting 7 seconds");
  delay(7000);

  // Start door shake
  Serial.println("Starting door shake");
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, HIGH);

  // Wait 5 seoncds
  Serial.println("Waiting 5 seconds");
  delay(5000);

  // End door shake
  Serial.println("Stopping door shake");
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);

  // Restart
  Serial.println("Sequence Finished, waiting 2 minutes");
  delay(120000);
  Serial.println("--- Restarting ---");
}
