// Initialization for rotation calculator
int sensorPin = A0; // select the input pin for LDR
int sensorValue = 0; // variable to store the value coming from the sensor
int diffThreshold = 30;
int prevAvg;
int currAvg;
int stateChangeCounter;
int revolutions;
int sensorVals[10];

int modernCount = 0;
int uniqueCount = 0;

bool isActivated;

// Initialization for LED strip
#include <Adafruit_NeoPixel.h>
#define LEDPIN 13
#define NUMPIXELS 40

// Set up  NeoPixel library, with pixels & pin.
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

int DELAYVAL = 5; // Time (in milliseconds) to pause between pixels
int color[] = {150, 0, 0}; // RGB values
int pulseSize = 1;

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
        Serial.println("F"); // "U" for uniqueness crank

        loopPulse();
      }
    }

    // if Serial input from other cranks or map
    if (Serial.available() > 0) {
      receivedChar = Serial.read();
      updateRevolutions(receivedChar);

      // for keyboard debugging, 'x' = rotation
      if (receivedChar == 'x') {
        revolutions++;
        stateChangeCounter = 0;

        // Serial output for Processing script
        Serial.println("F"); // "U" for uniqueness crank

        loopPulse();
      }
    }

    // Pause before reading from the LDR again
    // * comment out for testing with keyboard *
    //    delay(300);
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
void updateRevolutions(char received) {
  switch (received) {
    case 'M': // modernity rotation
      modernCount++;
      if (modernCount > threshold) {
        modernCount = 1;
      }
      break;
    case 'U': // uniqueness rotation
      uniqueCount++;
      if (uniqueCount > threshold) {
        uniqueCount = 1;
      }
      break;
    default:
      break;
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

void loopPulse() {
  updateDelay();
  updateColor();
  updatePulseSize();

  for (int i = 0; i < NUMPIXELS + pulseSize; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(color[0], color[1], color[2]));
    pixels.setPixelColor(i - pulseSize, pixels.Color(0, 0, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }

  pixels.clear();
}

// TODO: fix colors?!?!!
void updateColor() {
  color[0] = 255;
  color[1] = 0;
  color[2] = 0;
}

// from Unique crank
void updatePulseSize() {
  pulseSize = threshold - (uniqueCount % threshold);
}

// from Time crank
void updateDelay() {
  DELAYVAL = 2 * (threshold - (modernCount % threshold));
}
