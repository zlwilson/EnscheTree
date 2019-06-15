// Initialization for rotation calculator
int sensorPin = A0; // select the input pin for LDR
int sensorValue = 0; // variable to store the value coming from the sensor
int diffThreshold = 30;
int prevAvg;
int currAvg;
int stateChangeCounter;
int revolutions;
int sensorVals[10];

bool isActivated

// Initialization for LED strip
#include <Adafruit_NeoPixel.h>
#define LEDPIN 3
#define NUMPIXELS 30 // Length of one whole strip is 60 but we are using half strips

// Set up  NeoPixel library, with pixels & pin.
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

int DELAYVAL = 5; // Time (in milliseconds) to pause between pixels
int color[] = {150, 0, 0}; // RGB values
int multiplier = 1;
int uniqueness = 1;
int modernity = 1;
int feeling = 1;
int threshold = 20; // length of full cycle to get back to start, alternates at half

char receivedChar;

void setup() {
  Serial.begin(9600); // sets serial port for communication

  // Setup for rotation counter
  for (int i = 0; i < 10; i++) {
    sensorVals[i] = analogRead(sensorPin);
  }
  prevAvg = giveSensorAverage(sensorVals);
  currAvg = 0;
  stateChangeCounter = 0;
  revolutions = 0;

  // Setup for LED strip
  pixels.begin();
}

void loop() {
  checkIfActivated();

  // only count revolutions and blink if not in Activation stage
  if (!isActivated) {
    updateRevolutions();

    // Rotation counter code
    // make an array of the last 10 values read by the LDR
    for (int i = 0; i < 10; i++) {
      sensorVals[i] = analogRead(sensorPin);
    }

    // Calulate difference is running averages of last 10 values
    prevAvg = currAvg;
    currAvg = giveSensorAverage(sensorVals);
    float avgDiff = abs(prevAvg - currAvg);

    if (isChangeSignificant(avgDiff)) {
      stateChangeCounter++;

      // 2 state changes = 1 full rotations
      if (stateChangeCounter == 2) {
        revolutions++;
        stateChangeCounter = 0;

        // Serial output for Processing script
        Serial.print("U"); // "U" for uniqueness crank
        Serial.println();

        updateRevolutions();
        loopPulse();
      }
    }

    // Keyboard input for debugging
    // space bar => 1 rotation
    if (Serial.available() > 0) {
      receivedChar = Serial.read();
      if (receivedChar == "space") {
        revolutions++;
        stateChangeCounter = 0;

        // Serial output for Processing script
        Serial.print("U"); // "U" for uniqueness crank
        Serial.println();

        updateRevolutions();
        loopPulse();
      }
    }

    // Pause before reading from the LDR again
    delay(300);
  }
}

// Calculate if the difference between the current average and the previous average is significant
boolean isChangeSignificant(float averageDifference) {
  if (averageDifference > diffThreshold) {
    return true;
  } else {
    return false;
  }
}

// Calculate the average of the last X values stored in the array sensor values array
float giveSensorAverage(int sensorValuesArray[]) {
  float s;
  for (int i = 0; i < sizeof(sensorValuesArray); i++)
  {
    s += sensorValuesArray[i];
  }
  return s / sizeof(sensorValuesArray);
}

// Update the revolution counts for other cranks
void updateRevolutions() {
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    switch (receivedChar) {
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
}

void checkIfActivated() {
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == "A") {
      isActivated = true;
    } else if (receivedChar == "Z") { // Z = unactivate
      // TODO: can Processing send a message to the serial monitor to signal "unactivated"?
      isActivated = false;
    }
  }
}

// TODO:
// map modernity to speed, uniqueness to size, feeling to color
// DELAYVAL = speed
// color = color
// ?? = size
void loopPulse() {
  updateDelay();
  updateColor();

  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(NUMPIXELS - i - 4, pixels.Color(color[0], color[1], color[2]));
    pixels.setPixelColor(NUMPIXELS - i, pixels.Color(0, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }

  pixels.clear();
}

void updateDelay() {
  if (20 - revolutions / multiplier > 4) {
    DELAYVAL = 20 - revolutions / multiplier;
  }
}

void updateColor() {
  if (75 + revolutions / multiplier * 11 < 255) {
    color[0] = 75 + revolutions / multiplier * 11;
  } else {
    color[0] = 255;
  }

  if (revolutions / multiplier > 8 && revolutions / multiplier * 6 < 50) {
    color[1] = revolutions / multiplier * 6;
  } else {
    color[1] = 0;
  }

  if (revolutions / multiplier < 8 && revolutions / multiplier > 0) {
    color[2] = 30 - revolutions / multiplier * 4;
  } else {
    color[2] = 30;
  }
}
