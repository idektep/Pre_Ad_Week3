
void sendCurrentRobotArmState() {
  for (int i = 0; i < servoPins.size(); i++) {
    wsRobotArmInput.textAll(servoPins[i].servoName + "," + String(servoPins[i].initialPosition));
  }
  wsRobotArmInput.textAll(String("Record,") + (recordSteps ? "ON" : "OFF"));
  wsRobotArmInput.textAll(String("Play,") + (playRecordedSteps ? "ON" : "OFF"));
}

void writeServoValues(int servoIndex, int value) {
  // If recording steps, save the current step
  if (recordSteps) {
    RecordedStep recordedStep;
    unsigned long currentTime = millis();
    recordedStep.servoIndex = servoIndex;
    recordedStep.value = value;
    recordedStep.delayInStep = currentTime - previousTimeInMilli;
    recordedSteps.push_back(recordedStep);
    previousTimeInMilli = currentTime;
  }

  int Value = value;  

  // Set the PWM signal for the specified servo pin
  idektep.setPWM(servoPins[servoIndex].servoPin, 0, angleToPulse(Value));  // On at 0, off at Value
}


void playRecordedRobotArmSteps() {
  static unsigned long lastTime = 0;
  static size_t stepIndex = 0;

  if (recordedSteps.size() == 0) {
    return;
  }

  if (stepIndex < recordedSteps.size() && playRecordedSteps) {
    unsigned long currentTime = millis();
    RecordedStep &step = recordedSteps[stepIndex];

    if (currentTime - lastTime >= step.delayInStep * playbackSpeed) {
      writeServoValues(step.servoIndex, step.value);
      lastTime = currentTime;
      stepIndex++;
    }

  } else {
    // Reset playback when all steps are done
    stepIndex = 0;
  }
}
