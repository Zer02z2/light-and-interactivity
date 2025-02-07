int inputPin = A7;
int outputPin = 2;

const int numOfAverages = 3;
int averages[numOfAverages];

const int numOfInputs = 10;
int inputs[numOfInputs];

const int steps = 256;
int levelTable[steps];

int currentBrightness = 0;
int targetBrightness = 0;
float interpolate = 0.1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  fillLevelTable();
  for (int i = 0; i < numOfAverages; i++) {
    averages[i] = 0;
  }
  for (int i = 0; i < numOfInputs; i++) {
    inputs[i] = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int reading = analogRead(inputPin);
  int average = getAverage(reading);

  if (isChanging(average) == true) {
    targetBrightness = reading / (1024 / steps);
  }
  Serial.println(reading);

  currentBrightness += (targetBrightness - currentBrightness) * interpolate;
  analogWrite(outputPin, levelTable[currentBrightness]);

  // int reading = analogRead(inputPin);
  // int brightness = reading / 4;
  // analogWrite(outputPin, brightness);
}

void fillLevelTable() {

  // Calculate the scaling factor based on the
  // number of PWM steps you want:
  float scalingFactor = (steps * log10(2)) / (log10(steps));

  // iterate over the array and calculate the right value for it:
  for (int l = 0; l < steps; l++) {
    int lightLevel = pow(2, (l / scalingFactor)) - 1;
    levelTable[l] = lightLevel;
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
