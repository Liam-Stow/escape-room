constexpr int BUTTON_PIN = 2;
constexpr int SCREEN_RELAY_PIN = 3;
constexpr int DOOR_SHAKER_REPLAY_PIN = 4;
constexpr int PLAY_SOUND_1_PIN = 7;
constexpr int PLAY_SOUND_2_PIN = 6;
constexpr int SHAKE_ONCE_DURATION = 100;  // Decrease to shake faster
constexpr int NUMBER_OF_SHAKES = 50;

int buttonReadCounter = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SCREEN_RELAY_PIN, OUTPUT);
  pinMode(DOOR_SHAKER_REPLAY_PIN, OUTPUT);
  pinMode(PLAY_SOUND_1_PIN, OUTPUT);
  pinMode(PLAY_SOUND_2_PIN, OUTPUT);

  Reset();

  Serial.begin(115200);
}

void loop() {

  if (digitalRead(BUTTON_PIN) == HIGH) {
    buttonReadCounter++;
  } else {
    buttonReadCounter = 0;
  }

  if (buttonReadCounter > 10) {
    buttonReadCounter = 0;
    RunScareSequence();
  }

  delay(10);
}

void Reset() {
  // Set outputs to default state
  Serial.println("Resetting pinouts");
  digitalWrite(SCREEN_RELAY_PIN, LOW);
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);
  digitalWrite(PLAY_SOUND_1_PIN, HIGH);  // sounds plays when pin is set to LOW. Set to HIGH to begin with so nothing plays.
  digitalWrite(PLAY_SOUND_2_PIN, HIGH);
}

void RunScareSequence() {
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
  for (int i = 0; i < NUMBER_OF_SHAKES; i++) {
    digitalWrite(DOOR_SHAKER_REPLAY_PIN, HIGH);
    delay(SHAKE_ONCE_DURATION);
    digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);
    delay(SHAKE_ONCE_DURATION);
  }

  // Restart
  Serial.println("Sequence Finished, waiting 2 minutes");
  delay(120000);
  Reset();
}