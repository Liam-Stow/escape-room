constexpr int SWITCH_PIN = 13;
constexpr int LED_RELAY_PIN = 4;
constexpr int PLAY_SOUND_1_PIN = 6;
constexpr int PLAY_SOUND_2_PIN = 7;

void setup()
{
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_RELAY_PIN, OUTPUT);
    pinMode(PLAY_SOUND_1_PIN, OUTPUT);
    pinMode(PLAY_SOUND_2_PIN, OUTPUT);

    Serial.begin(115200);

    // Set outputs to default state
    digitalWrite(LED_RELAY_PIN, LOW);
    digitalWrite(PLAY_SOUND_1_PIN, HIGH); // sounds play when pin is set to LOW. Set to HIGH to begin with so nothing plays.
    digitalWrite(PLAY_SOUND_2_PIN, HIGH);
}

void loop()
{
    if (digitalRead(SWITCH_PIN) == LOW) {
        digitalWrite(PLAY_SOUND_1_PIN, LOW);
        digitalWrite(LED_RELAY_PIN, HIGH);
        Serial.println("Switch detected - playing sound");
    } else {
        digitalWrite(LED_RELAY_PIN, LOW);
        digitalWrite(PLAY_SOUND_1_PIN, HIGH);
        Serial.println("Switch not detected - not playing sound");
    }

    delay(100);
}
