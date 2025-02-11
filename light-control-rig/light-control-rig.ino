int inputPin = A1;
int outputPin = 0;
int buttonPin = 4;

const int numOfAverages = 3;
int averages[numOfAverages];

const int numOfInputs = 5;
int inputs[numOfInputs];

const int steps = 256;

int currentProgress = 0;
int targetProgress = 0;

bool lampIsOn = false;

long lastMillis = 0;
int interval = 5;

int threshold = 60;

void setup() {
  // put your setup code here, to run once:
  //fillLevelTable();
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

  if (isChanging(average) == true || reading - currentProgress > threshold) {
    targetProgress = reading / (1024 / steps);
  }
  if (currentProgress != targetProgress) {
    if (millis() - lastMillis > interval) {
      if (currentProgress < targetProgress) {
        currentProgress++;
      } else {
        currentProgress--;
      }
      lastMillis = millis();
    }
  }

  int brightnessNow = getBrightness(currentProgress);

  int buttonReading = digitalRead(buttonPin);
  if ((buttonReading == HIGH && lampIsOn == false) || (buttonReading == LOW && lampIsOn == true)) {
    dim(brightnessNow, lampIsOn);
    lampIsOn = !lampIsOn;
  }

  if (lampIsOn) {
    analogWrite(outputPin, brightnessNow);
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

int getBrightness(int progress) {
  float lightLevel = pow(progress, 2);
  lightLevel = map(lightLevel, 0, 65535, 5, steps);
  return (int)lightLevel;
}

void dim(int brightness, bool isOn) {
  int currentSteps = 0;
  while (currentSteps != steps) {
    currentSteps++;
    float lightLevel;
    if (isOn == true) {
      lightLevel = pow(steps - currentSteps, 2);
    } else if (isOn == false) {
      lightLevel = pow(currentSteps, 2);
    }
    lightLevel = map(lightLevel, 0, 65535, 0, brightness);
    analogWrite(outputPin, (int)lightLevel);
    delay(5);
  }
}
