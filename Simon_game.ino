#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define NUM_BUTTONS 4
#define NUM_LEDS 4
#define START_BUTTON 11
#define BUZZER_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPins[NUM_LEDS] = {2, 3, 4, 5};
int buttonPins[NUM_BUTTONS] = {6, 7, 8, 9};
int frequencies[NUM_LEDS] = {200, 300, 400, 500};

int sequence[100];
int sequenceLength = 0;
int playerIndex = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // ดับทุกดวง
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  waitForStart();
}

void loop() {
  playSequence();
  playerTurn();
}

void waitForStart() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press White");
  lcd.setCursor(0, 1);
  lcd.print("button to start");

  while (digitalRead(START_BUTTON) == HIGH) {
    delay(100);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  delay(1000);
  startGame();
}

void startGame() {
  sequenceLength = 0;
  playerIndex = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: 1");
  addToSequence();
}

void addToSequence() {
  sequence[sequenceLength] = random(0, NUM_LEDS);
  sequenceLength++;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(sequenceLength);
  delay(500);
}

void playSequence() {
  for (int i = 0; i < sequenceLength; i++) {
    int led = sequence[i];

    // ดับไฟทุกดวงก่อน
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(ledPins[j], LOW);
    }

    digitalWrite(ledPins[led], HIGH);
    tone(BUZZER_PIN, frequencies[led], 300);
    delay(500);
    digitalWrite(ledPins[led], LOW);
    delay(300);
  }
}

void playerTurn() {
  playerIndex = 0;

  while (playerIndex < sequenceLength) {
    int pressedButton = getButtonPress();
    if (pressedButton == -1) continue;

    if (pressedButton == sequence[playerIndex]) {
      digitalWrite(ledPins[pressedButton], HIGH);
      tone(BUZZER_PIN, frequencies[pressedButton], 200);
      delay(200);
      digitalWrite(ledPins[pressedButton], LOW);
      playerIndex++;
    } else {
      gameOver();
      return;
    }
  }

  delay(500);
  addToSequence();
}

int getButtonPress() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      delay(200);
      while (digitalRead(buttonPins[i]) == LOW); // wait for release
      return i;
    }
  }
  return -1;
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Final Level: ");
  lcd.print(sequenceLength);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(ledPins[j], HIGH);
    }
    tone(BUZZER_PIN, 600, 500);
    delay(500);
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(ledPins[j], LOW);
    }
    delay(500);
  }

  delay(2000);
  waitForStart();
}
