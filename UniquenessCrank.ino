#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the Neostrip?
#define PIN        13 // On Trinket or Gemma, suggest changing this to 1

// How many Neostrip are attached to the Arduino?
#define NUMPIXEL 30 // Popular NeoPixel ring size

#define SINE true
#define PI 3.1415926535897932384626433832795

// When setting up the NeoPixel library, we tell it how many strip,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXEL, PIN, NEO_GRBW + NEO_KHZ800);

int pulsesize = 1; // uniqueness
int delayval = 5; // modernity
int color[] = {255, 0, 0}; // mood (in RGB)

int feel = 0;
int unique = 0;
int modern = 0;

int threshold = 20; // number of revolutions per full cycle



// Initialization for rotation calculator
int sensorPin = A0; // select the input pin for LDR
int sensorValue = 0; // variable to store the value coming from the sensor
int diffThreshold = 30;
int prevAvg;
int currAvg;
int stateChangeCounter;
int revolutions;
int sensorVals[10];




char incomingByte;   // for incoming serial data

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(13, OUTPUT);    // Open serial monitor with shift + cmd + M (on Mac)
  digitalWrite(13, LOW);

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.


  // Setup for rotation counter
  for (int i = 0; i < 10; i++) {
    sensorVals[i] = analogRead(sensorPin);
  }
  prevAvg = giveSensorAverage(sensorVals);
  currAvg = 0;
  stateChangeCounter = 0;
  revolutions = 0;



  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {

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
      Serial.println("u"); // "u" for uniqueness crank

      unique++;

      showLED();
    }
  }

  // if Serial input from other cranks or map
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    updateRevolutions(incomingByte);
    showLED();
  }
  delay(300);
}

void updateLED() {

  if (SINE) {

    double p = (threshold + 1 - rot(unique)) / threshold * (NUMPIXEL - 10);
    pulsesize = (int) (round(p));
    double d = 5 + rot(modern) * 20 / threshold;
    delayval = (int) (round(d));

    double r = 270 - (10 + 245 * rot(feel) / threshold);
    color[0] = (int) (round(r));
    double g = 10 - (-40 + 60 * rot(feel) / threshold);
    color[1] = (int) (round(g));
    if (color[1] < 0) {
      color[1] = 0;
    }
    double b = 50 * rot(feel) / threshold - 1;
    color[2] = (int) (round(b));

  } else {
    double p = (threshold - (unique % threshold)) / threshold * (NUMPIXEL - 10);
    pulsesize = (int) (round(p));
    double d = (threshold + 5) - (modern % threshold);
    delayval = (int) (round(d));
    double r = 10 + (245 / threshold) * (feel % threshold);
    double g = -40 + (80 / threshold) * (feel % threshold);
    double b = 40 - (40 / threshold) * (feel % threshold);
    color[0] = (int) (round(r));
    color[1] = (int) (round(g));
    if (color[1] < 0) {
      color[1] = 0;
    }
    color[2] = (int) (round(b));
  }

  if (pulsesize > NUMPIXEL - 10) {
    pulsesize = NUMPIXEL - 10;
  }


  //printInfo();
}

// f = feeling: color
// m = modernity: speed
// u = unique: pulse size
void showLED() {
  updateLED();

  for (int i = 0; i < NUMPIXEL + pulsesize; i++) { // For each pixel...
    // strip.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    strip.setPixelColor(i, color[0], color[1], color[2], 0);
    strip.setPixelColor(i - pulsesize, 0, 0, 0, 0);

    strip.show();   // Send the updated pixel colors to the hardware.

    delay(delayval); // Pause before next pass through loop
  }
  strip.clear();
}

double rot(int x) { // sinusoid number between 1 and threshold
  return 1 + ((threshold - 1) * ((cos((x * 2 * PI) / threshold) + 1) / 2));
}

void printInfo() {
  Serial.print("Counts: ");
  Serial.print(modern);
  Serial.print(" ");
  Serial.print(feel);
  Serial.print(" ");
  Serial.print(unique);
  Serial.print(", rgb(");
  Serial.print(color[0]);
  Serial.print(", ");
  Serial.print(color[1]);
  Serial.print(", ");
  Serial.print(color[2]);
  Serial.print("), pulsesize: ");
  Serial.print(pulsesize);
  Serial.print(", delay: ");
  Serial.println(delayval);
}

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

void updateRevolutions(char received) {
  switch (received) {
    case 'm': // modernity rotation
      modern++;
      break;
    case 'u': // feeling rotation
      unique++;
      break;
    case 'f': // feeling rotation
      feel++;
      break;
    default:
      break;
  }
}
