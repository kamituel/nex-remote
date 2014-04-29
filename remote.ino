/* Button debouncer module */

int DEBOUNCE_DELAY = 50;
typedef struct ButtonDebouncer {
  int pinButton;
  
  int buttonState;
  int lastButtonState;
  long lastDebounceTime;
  int debounceDelay;
  
  void (*onButtonPressed)();
  
  void init(int pin) {
    pinButton = pin;
    lastDebounceTime = 0;
    lastButtonState = LOW;
    debounceDelay = DEBOUNCE_DELAY;
  }
  
  void debounce() {
    int reading = digitalRead(pinButton);

    if (reading != lastButtonState) {
      Serial.print("debounce! ");
      Serial.print(lastButtonState);
      Serial.print(" ");
     Serial.print(reading); 
     Serial.print(" - ");
     Serial.print(HIGH);
     Serial.print(" ");
     Serial.print(LOW);
     Serial.print("\n");
      lastDebounceTime = millis();
    }
        
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;
        
        if (buttonState == HIGH) {
          onButtonPressed();
        }
      }
    }
    
    lastButtonState = reading;
  }
} ButtonDebouncer;

/* Pin assigments */
int pinButtonShutter = 2;
int pinButtonShutterDelayed = 4;
int pinButtonIntervalIncrease = 8;
int pinButtonIntervalDecrease = 13;
int pinButtonIntervalToggle = 7;

ButtonDebouncer shutterButton;
ButtonDebouncer shutterDelayedButton;
ButtonDebouncer intervalIncreaseButton;
ButtonDebouncer intervalDecreaseButton;
ButtonDebouncer intervalToggleButton;

int pinIRLED = 12; // assign the Infrared emitter/ diode to pin 13
int LEDgreen = 13; // use onboard led for battery status
int batteryIn = 0; // set pin to get power data from
int batLevel = 0; // variable to get details on battery power level from analog input
int lowPower = 0; //count to keep track of lowpower state

// http://mjr.org/RMT-DSLR1.lirc
int SHUTTER[] = {2336, 646, 1168, 621, 572, 621, 1168, 621, 1168, 621, 572, 621, 1168, 621, 572, 646, 572, 621, 1168, 621, 572, 621, 1168, 621, 1168, 621, 1168, 621, 572, 621, 572, 646, 572, 621, 1168, 621, 1168, 621, 1168, 621, 1168, 11008, 2336, 621, 1168, 621, 572, 621, 1168, 621, 1168, 621, 572, 621, 1168, 621, 572, 646, 572, 621, 1168, 621, 572, 621, 1168, 621, 1168, 621, 1168, 621, 572, 621, 572, 646, 572, 621, 1168, 621, 1168, 621, 1168, 621, 1168, 11008, 2336, 621, 1168, 621, 572, 621, 1168, 621, 1168, 621, 572, 621, 1168, 621, 572, 646, 572, 621, 1168, 621, 572, 621, 1168, 621, 1168, 621, 1093, 696, 572, 621, 572, 621, 572, 621, 572, 1218, 497, 1292, 422, 1367, 373, 11803, 298, 2659, 199, 1590, 174, 1019, 174, 1615, 174, 1615, 149, 1044, 149, 1640, 124, 1093, 149, 1044, 124, 1665, 124, 1068, 124, 1665, 99, 1690, 99, 1690, 99, 1093, 99, 1118, 99, 1093, 99, 1690, 99, 1690, 75, 1715, 75, 12101, 149, 2833, 75, 1715, 75, 1118, 75, 1715, 75, 1715, 75, 1118, 75, 1715, 75, 1118, 99, 1093, 99, 1690, 99, 1093, 99, 1690, 99, 1690, 99, 1690, 99, 1093, 99, 1118, 99, 1093, 99, 1690, 99, 1690, 99, 1690, 99, 646, -1};
int SHUTTER2[] = {2336, 646, 1168, 621, 1168, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 572, 646, 572, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 1168, 621, 572, 621,  572, 646, 572, 621, 1168, 621,  1168, 621, 1168, 621, 1168, 10411,  2336, 621, 1168, 621, 1168, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 572, 646, 572, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 1168, 621, 572, 621,  572, 646, 572, 621, 1168, 621,  1168, 621, 1168, 621, 1168, 10436,  2336, 646, 1168, 621, 1168, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 572, 646, 572, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 1168, 621, 572, 621,  572, 646, 572, 621, 1168, 621,  1168, 621, 1168, 621, 1168, 10411,  2336, 646, 1168, 621, 1168, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 572, 646, 572, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 1168, 621, 572, 621,  572, 646, 572, 621, 1168, 621,  1168, 621, 1168, 621, 1168, 10411,  2336, 621, 1168, 621, 1168, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 572, 646, 572, 621,  1168, 621, 572, 621, 1168, 621,  1168, 621, 1168, 621, 572, 621,  572, 646, 572, 621, 1168, 621,  1168, 621, 1168, 621, 1168, -1};

/* Interval shutter */
int MAX_INTERVAL = 3600;
int MIN_INTERVAL = 2;
int DEFAULT_INTERVAL = 5;
typedef struct TimelapseTrigger {
  int interval; // sec
  boolean running;
  long lastShutterTime;
  
  void init() {
    interval = DEFAULT_INTERVAL;
    running = false;
    lastShutterTime = 0;
  }
  
  void loop() {
    if (running) {
      if (lastShutterTime + interval * 1000 < millis()) {
        lastShutterTime = millis();
        sendCommand(SHUTTER);
        sendCommand(SHUTTER);
      }
    }
  }
  
  void toggle() {
    running = !running;
  }
  
  void increaseInterval() {
    if (interval < MAX_INTERVAL) {
      ++interval;
    }
  }
  
  void decreaseInterval() {
    if (interval > MIN_INTERVAL) {
      --interval;
    }
  }
} TimelapseTrigger;

TimelapseTrigger timelapseTrigger;

void setup() {
  Serial.begin(9600);
  
  pinMode(pinIRLED, OUTPUT);
  pinMode(pinButtonShutter, INPUT);
  pinMode(pinButtonShutterDelayed, INPUT);
  pinMode(pinButtonIntervalIncrease, INPUT);
  pinMode(pinButtonIntervalDecrease, INPUT);
  pinMode(pinButtonIntervalToggle, INPUT);
  
  shutterButton.init(pinButtonShutter);
  shutterButton.onButtonPressed = commandShutter;
  
  shutterDelayedButton.init(pinButtonShutterDelayed);
  shutterDelayedButton.onButtonPressed = commandShutterDelayed;
  
  timelapseTrigger.init();
  
  intervalIncreaseButton.init(pinButtonIntervalIncrease);
  intervalIncreaseButton.onButtonPressed = increaseInterval;
  
  intervalDecreaseButton.init(pinButtonIntervalDecrease);
  intervalDecreaseButton.onButtonPressed = decreaseInterval;
  
  intervalToggleButton.init(pinButtonIntervalToggle);
  intervalToggleButton.onButtonPressed = toggleInterval;
}

// sets the pulse of the IR signal.
void pulseON(int pulseTime) {
  unsigned long endPulse = micros() + pulseTime;        // create the microseconds to pulse for
  while( micros() < endPulse) {
    digitalWrite(pinIRLED, HIGH);                       // turn IR on
    delayMicroseconds(13);                              // half the clock cycle for 38Khz (26.32Ã�10-6s) - e.g. the 'on' part of our wave
    digitalWrite(pinIRLED, LOW);                        // turn IR off
    delayMicroseconds(13);                              // delay for the other half of the cycle to generate wave/ oscillation
  }
}
void pulseOFF(unsigned long startDelay) {
  unsigned long endDelay = micros() + startDelay;       // create the microseconds to delay for
  while(micros() < endDelay);
}

void sendCommand(int command[]) {
  int c;
  for (c = 0; command[c] >= 0; c += 1) {
    if (c % 2 == 0) {
      pulseON(command[c]);
    } else {
      pulseOFF(command[c]);
    }
  }
}

void takePicture() {
  sendCommand(SHUTTER);
  sendCommand(SHUTTER);
}

void batterytestdelay(unsigned long delaytime){  // Function to check battery level and flash the led if its low battery
  long tempdelay = 0;
  while(delaytime > tempdelay){
    delay(100);
    tempdelay = tempdelay + 100;
     batLevel = analogRead(batteryIn);
    if (batLevel < 720){
      lowPower++;
    }
    if (lowPower > 100){
      lowPower = 0;
    }
    if (lowPower < 50){
      digitalWrite(LEDgreen, LOW);
    }
    else{
      digitalWrite(LEDgreen, HIGH);
    }
  }
}

void loop() {
  //shutterButton.debounce();
  //shutterDelayedButton.debounce();
  intervalIncreaseButton.debounce();
  //intervalDecreaseButton.debounce();
  //intervalToggleButton.debounce();
  //timelapseTrigger.loop();

  //buttonShutterDelayedHandle();
  
  //digitalWrite(LEDgreen, HIGH);
  //takePicture();  // take the picture
  //digitalWrite(LEDgreen, LOW);
  //batterytestdelay(5000);                                          // delay in milliseconds which allows us to do timelapse photography - 1 second = 1000 milliseconds. This delay has been passed to the battery testing function
}

void commandShutter() {
  Serial.print("Shutter released now\n");
  sendCommand(SHUTTER);
  sendCommand(SHUTTER);
}

void commandShutterDelayed() {
  Serial.print("Shutter release in 2 sec\n");
  sendCommand(SHUTTER2);
  sendCommand(SHUTTER2);
}

void increaseInterval() {
  Serial.print("Increase interval\n");
  timelapseTrigger.increaseInterval();
  Serial.print(timelapseTrigger.interval);
  Serial.print("\n");
}

void decreaseInterval() {
  Serial.print("Decrease interval\n");
  timelapseTrigger.decreaseInterval();
  Serial.print(timelapseTrigger.interval);
  Serial.print("\n");
}

void toggleInterval() {
  Serial.print("Toggle interval\n");
  timelapseTrigger.toggle();
  Serial.print(timelapseTrigger.interval);
  Serial.print("\n");
}
