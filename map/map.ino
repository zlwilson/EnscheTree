// Software initialization
int uniqueCount = 1;
int modernCount = 1;
int feelingCount = 1;
int threshold = 20;
bool isUnique = false;
bool isModern = false;
bool isFeeling = false;

char receivedChar;
int buttonState;

// Hardware initialize
#define BTNPIN 10

// keep pins in a line to make it easy to iterate through
#define LEDPIN1 1 // old, boring, sad
#define LEDPIN2 2 // old, boring, happy
#define LEDPIN3 3 // old, exciting, sad
#define LEDPIN4 4 // old, exciting, happy
#define LEDPIN5 5 // modern, boring, sad
#define LEDPIN6 6 // modern, boring, happy
#define LEDPIN7 7 // modern, exciting, sad
#define LEDPIN8 8 // modern, exciting, happy

// TODO: setup of LED ring from Alfred
#define LEDRINGPIN = 0; // LED for button backlight

void setup() {
  pinMode(BTNPIN, INPUT);
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);
  pinMode(LEDPIN4, OUTPUT);
  pinMode(LEDPIN5, OUTPUT);
  pinMode(LEDPIN7, OUTPUT);
  pinMode(LEDPIN8, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  updateVals();

  // switch stories
  int selectedStory = getStory();
  // turn off all other LEDs
  for (int i = LEDPIN1; i <= LEDPIN8; i++) {
    digitalWrite(i, LOW);
  }
  // turn on selectedStory LED
  digitalWrite(selectedStory, HIGH);

  // activation button
  checkButtonPress();
}

// update all story values
void updateVals() {
  // update revolution counts
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    switch (receivedChar) {
      case 'U': // uniqueness rotation
        uniqueCount++;
        if (uniqueCount > threshold) {
          uniqueCount = 1;
        }
        break;
      case 'M': // modernity rotation
        modernCount++;
        if (modernCount > threshold) {
          modernCount = 1;
        }
        break;
      case 'F': // feeling rotation
        feelingCount++;
        if (feelingCount > threshold) {
          feelingCount = 1;
        }
        break;
      default:
        break;
    }
  }

  // update selections based on the threshold
  if (uniqueCount > threshold / 2) {
    isUnique = true;
  } else {
    isUnique = false;
  }
  if (modernCount > threshold / 2) {
    isModern = true;
  } else {
    isModern = false;
  }
  if (feelingCount > threshold / 2) {
    isFeeling = true;
  } else {
    isFeeling = false;
  }
}

// pick story LED
int getStory() {
  if (isUnique) {
    if (isModern) {
      if (isFeeling) {
        return LEDPIN8; // modern, exciting, happy
      } else {
        return LEDPIN7; // modern, exciting, sad
      }
    } else {
      if (isFeeling) {
        return LEDPIN4; // old, exciting, happy
      } else {
        return LEDPIN3; // old, exciting, sad
      }
    }
  } else {
    if (isModern) {
      if (isFeeling) {
        return LEDPIN6; // modern, boring, happy
      } else {
        return LEDPIN5; // modern, boring, sad
      }
    } else {
      if (isFeeling) {
        return LEDPIN2; // old, boring, happy
      } else {
        return LEDPIN1; // old, boring, sad
      }
    }
  }
}

void checkButtonPress() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(BTNPIN);

  if (buttonState == HIGH) { // Activation!
    // Send info to Processing
    Serial.write("A"); // "A" for activation!!

    // Do stuff with LEDs...
    for (int i = LEDPIN1; i <= LEDPIN8; i++) {
      digitalWrite(i, LOW);
    }
  }
}
