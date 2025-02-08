int inputPin = A7;
int outputPin = 2;
int buttonPin = 3;

const int numOfAverages = 5;
int averages[numOfAverages];

const int numOfInputs = 10;
int inputs[numOfInputs];

const int steps = 256;
int levelTable[steps];

int currentBrightness = 0;
int targetBrightness = 0;
//float interpolate = 0.05;

int lastButtonReading = 0;
bool lampIsOn = false;

void setup() {
  // put your setup code here, to run once:
  fillLevelTable();
  for (int i = 0; i < numOfAverages; i++) {
    averages[i] = 0;
  }
  for (int i = 0; i < numOfInputs; i++) {
    inputs[i] = 0;
  }
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int reading = analogRead(inputPin);

  int average = getAverage(reading);

  if (isChanging(average) == true) {
    targetBrightness = reading / (1024 / steps);
  }
  if (currentBrightness != targetBrightness) {
    if (currentBrightness < targetBrightness) {
      currentBrightness ++;
    } else {
      currentBrightness --;
    }
    delay(10);
  }

  int buttonReading = digitalRead(buttonPin);
  if (buttonReading != lastButtonReading && buttonReading == HIGH) {
    dim(levelTable[currentBrightness], lampIsOn);
    lampIsOn = !lampIsOn;
  }
  lastButtonReading = buttonReading;

  if (lampIsOn) {
    int chosenBrightness = levelTable[currentBrightness];
    analogWrite(outputPin, chosenBrightness);
  } else {
    analogWrite(outputPin, 0);
  }
}

bool isChanging(int reading) {
  for (int i = 0; i < numOfAverages - 1; i++) {
    averages[i] = averages[i + 1];
  }
  averages[numOfAverages - 1] = reading;

  bool isChanging = false;
  for (int i = 0; i < numOfAverages - 1; i++) {
    if (averages[i] < averages[i + 1]) {
      isChanging = true;
    } else {
      isChanging = false;
      break;
    }
  }
  if (isChanging == true) return isChanging;

  for (int i = 0; i < numOfAverages - 1; i++) {
    if (averages[i] > averages[i + 1]) {
      isChanging = true;
    } else {
      isChanging = false;
      break;
    }
  }
  return isChanging;
}

int getAverage(int reading) {
  int sum = 0;
  for (int i = 0; i < numOfInputs - 1; i++) {
    inputs[i] = inputs[i + 1];
    sum += inputs[i];
  }
  inputs[numOfInputs - 1] = reading;
  sum += reading;
  int result = sum / numOfInputs;
  return result;
}

// sin curve
// void fillLevelTable() {
//   // iterate over the array and calculate the right value for it:
//   for (int l = 0; l < steps; l++) {
//     // map input to a 0-360 range:
//     int angle = map(l, 0, steps, 0, 360);
//     // convert to radians:
//     float lightLevel = angle * PI / 360;
//     // now subtract PI/2 to offset by 90 degrees, so yuu can start fade at 0:
//     lightLevel -= PI / 2;
//     // get the sine of that:
//     lightLevel = sin(lightLevel);
//     // now you have -1 to 1. Add 1 to get 0 to 2:
//     lightLevel += 1;
//     // multiply to get 0-255:
//     lightLevel *= (steps - 1) / 2;
//     // put it in the array:
//     levelTable[l] = int(lightLevel);
//   }
// }

//Exponentioal
// void fillLevelTable() {

//   // Calculate the scaling factor based on the
//   // number of PWM steps you want:
//   float scalingFactor = (steps * log10(2)) / (log10(steps));

//   // iterate over the array and calculate the right value for it:
//   for (int l = 0; l < steps; l++) {
//     int lightLevel = pow(2, (l / scalingFactor)) - 1;
//     lightLevel = map(lightLevel, 0, steps, 5, steps);
//     levelTable[l] = lightLevel;
//   }
// }

//Xsquare
void fillLevelTable() {
  // iterate over the array and calculate the right value for it:
  for (int l = 0; l <= steps; l++) {
    // square the current value:
    float lightLevel = pow(l, 2);
    // map the result back to the resolution range:
    lightLevel = map(lightLevel, 0, 65535, 5, steps - 1);
    levelTable[l] = lightLevel;
  }
}

void dim(int brightness, bool isOn) {
  int table[steps];
  for (int l = 0; l <= steps - 1; l++) {
    float lightLevel = pow(l, 2);
    lightLevel = map(lightLevel, 0, 65535, 0, brightness);
    table[l] = lightLevel;
  }
  int currentSteps = 0;
  while (currentSteps != steps) {
    currentSteps++;
    int chosenBrightness;
    if (isOn == true) {
      chosenBrightness = (int)table[steps - 1 - currentSteps];
    } else if (isOn == false) {
      chosenBrightness = (int)table[currentSteps];
    }
    analogWrite(outputPin, chosenBrightness);
    delay(5);
  }
}
