// Requires DIP Switches to be in 'I/O Independent Mode 1':
// Switch 1 = ON
// Switch 2 = ON
// Switch 3 = OFF
// This will cause the controller to stop all sounds once all input pins are HIGH.

// Pin numbers
constexpr int SWITCH_PIN = 13;
constexpr int LED_RELAY_PIN = 3;
constexpr int PLAY_SOUND_PIN = 4;
constexpr int PLAY_SOUND_2_PIN = 5; // Only included because I think I saw an extra wire going to the audio controller in the photo, we want to force this to HIGH so it doesn't mess with anything.

// Output commands
constexpr int PLAY_SOUND_COMMAND = LOW;
constexpr int STOP_SOUND_COMMAND = HIGH;
constexpr int LED_ON_COMMAND = HIGH;
constexpr int LED_OFF_COMMAND = LOW;

// Inputs
constexpr int SWITCH_ON = LOW;
constexpr int SWITCH_OFF = HIGH;

// Timing
constexpr int SOUND_DURATION_MILLISECONDS = 60000;
constexpr int LOOP_TIME_MILLISECONDS = 100;

enum State { OFF, PLAYING, FINISHED };
State state = OFF;
int playTimer = 0;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_RELAY_PIN, OUTPUT);
  pinMode(PLAY_SOUND_PIN, OUTPUT);
  pinMode(PLAY_SOUND_2_PIN, OUTPUT);

  Serial.begin(115200);

  // Set outputs to default state
  digitalWrite(LED_RELAY_PIN, LED_OFF_COMMAND);
  digitalWrite(PLAY_SOUND_PIN, STOP_SOUND_COMMAND);
  digitalWrite(PLAY_SOUND_2_PIN, STOP_SOUND_COMMAND);
}

void loop() {
  switch (state) {
    case OFF:
      digitalWrite(PLAY_SOUND_PIN, STOP_SOUND_COMMAND);
      digitalWrite(LED_RELAY_PIN, LED_OFF_COMMAND);
      playTimer = 0;
      if (digitalRead(SWITCH_PIN) == SWITCH_ON) {
        Serial.println("Switch detected - playing sound");
        state = PLAYING;
      }
      break;

    case PLAYING:
      digitalWrite(PLAY_SOUND_PIN, PLAY_SOUND_COMMAND);
      digitalWrite(LED_RELAY_PIN, LED_ON_COMMAND);
      if (digitalRead(SWITCH_PIN) == SWITCH_OFF) {
        Serial.println("Switch off detected - turning sound off");
        state = OFF;
      }
      if (playTimer >= SOUND_DURATION_MILLISECONDS) {
        Serial.println("Sound duration expired - turning off sound");
        state = FINISHED;
      }
      playTimer += LOOP_TIME_MILLISECONDS;
      break;

    case FINISHED:
      digitalWrite(PLAY_SOUND_PIN, STOP_SOUND_COMMAND);
      digitalWrite(LED_RELAY_PIN, LED_OFF_COMMAND);
      playTimer = 0;
      if (digitalRead(SWITCH_PIN) == SWITCH_OFF) {
        Serial.println("Switch off detected - resetting");
        state = OFF;
      }
      break;
  }

  delay(LOOP_TIME_MILLISECONDS);
}
