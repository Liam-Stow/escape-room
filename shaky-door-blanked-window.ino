#include "DYPlayer/src/DYPlayerArduino.h"

constexpr int BUTTON_PIN = 1;
constexpr int SCREEN_RELAY_PIN = 2;
constexpr int DOOR_SHAKER_REPLAY_PIN = 3;
DY::Player audioPlayer;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SCREEN_RELAY_PIN, OUTPUT);
  pinMode(DOOR_SHAKER_REPLAY_PIN, OUTPUT);

  digitalWrite(SCREEN_RELAY_PIN, LOW);
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);
  audioPlayer.begin();
  audioPlayer.setVolume(15); // vol from 0 -> 30 (15 = 50%)

  // Wait for button press
  while (digitalRead(BUTTON_PIN) == LOW) {}

  // Wait 3 seconds
  delay(3000);

  // Play sound 1
  audioPlayer.playSpecified(1);

  // Wait 5 seconds
  delay(5000);

  // Drop screen
  digitalWrite(SCREEN_RELAY_PIN, HIGH);

  // Play sound 2
  audioPlayer.playSpecified(2);

  // Wait 3 seconds
  delay(3000);

  // Start door shake
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, HIGH);

  // Wait 5 seoncds
  delay(5000);

  // End door shake
  digitalWrite(DOOR_SHAKER_REPLAY_PIN, LOW);
}

void loop() {}
