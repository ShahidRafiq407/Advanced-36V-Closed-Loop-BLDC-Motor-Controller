#include <Arduino.h>

const int throttlePin = 34;

// Hall Sensors
const int hA = 22, hB = 19, hC = 21;

// MOSFETs
const int ah = 25, al = 26, bh = 27, bl = 14, ch = 32, cl = 33;

void setup() {
  Serial.begin(115200);

  // Sab MOSFETs OFF
  int mosfets[] = {ah, al, bh, bl, ch, cl};
  for(int p : mosfets) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }

  // Seeti band (20kHz Silent PWM)
  analogWriteFrequency(20000);

  // Inputs Setup
  pinMode(throttlePin, INPUT);
  pinMode(hA, INPUT_PULLUP);
  pinMode(hB, INPUT_PULLUP);
  pinMode(hC, INPUT_PULLUP);
}

// Sensored Commutation Function
void setPhasePWM(int pwmA, bool L_A, int pwmB, bool L_B, int pwmC, bool L_C) {
  analogWrite(ah, 0); analogWrite(bh, 0); analogWrite(ch, 0);
  digitalWrite(al, LOW); digitalWrite(bl, LOW); digitalWrite(cl, LOW);
  
  delayMicroseconds(50); 

  digitalWrite(al, L_A); digitalWrite(bl, L_B); digitalWrite(cl, L_C);
  analogWrite(ah, pwmA); analogWrite(bh, pwmB); analogWrite(ch, pwmC);
}

void loop() {
  int adcValue = analogRead(throttlePin);
  
  // Throttle 0 par motor band (1100 ki deadzone limit)
  if(adcValue < 1100) {
      setPhasePWM(0, LOW, 0, LOW, 0, LOW);
      return; 
  }

  // Throttle Mapping: Minimum PWM 100 rakha hai taaki stall na ho
  int speedPWM = map(adcValue, 1100, 3200, 100, 255);
  if(speedPWM > 255) speedPWM = 255;
  if(speedPWM < 100) speedPWM = 100; // Torque on startup

  // Hall Sensor Read
  int state = (digitalRead(hA) << 2) | (digitalRead(hB) << 1) | digitalRead(hC);

  // -----------------------------------------------------
  // ⬇️ TUNING SECTION (YE DO NUMBERS CHANGE KARNE HAIN) ⬇️
  // -----------------------------------------------------
  int dir = 1;      // Pehle 1 rakhein. Agar 0-5 tak theek na ho, toh -1 kar dein!
  int offset = 0;   // Ise 0, 1, 2, 3, 4, 5 tak change karke try karein
  // -----------------------------------------------------

  int sequence[6] = {4, 6, 2, 3, 1, 5};
  int pos = -1;
  for(int i = 0; i < 6; i++) {
    if(sequence[i] == state) pos = i;
  }

  // Closed Loop Commutation!
  if(pos != -1) {
    int stepIndex;
    if (dir == 1) {
       stepIndex = (pos + offset) % 6;
    } else {
       stepIndex = (pos - offset + 6) % 6; // Reverse Mapping
    }

    // Exact open-loop wale steps (PWM ke sath)
    switch(stepIndex) {
      case 0: setPhasePWM(speedPWM, LOW,        0, HIGH,       0, LOW);  break; // A+, B-
      case 1: setPhasePWM(speedPWM, LOW,        0, LOW,        0, HIGH); break; // A+, C-
      case 2: setPhasePWM(       0, LOW, speedPWM, LOW,        0, HIGH); break; // B+, C-
      case 3: setPhasePWM(       0, HIGH,speedPWM, LOW,        0, LOW);  break; // B+, A-
      case 4: setPhasePWM(       0, HIGH,       0, LOW, speedPWM, LOW);  break; // C+, A-
      case 5: setPhasePWM(       0, LOW,        0, HIGH,speedPWM, LOW);  break; // C+, B-
    }
  } else {
    setPhasePWM(0, LOW, 0, LOW, 0, LOW);
  }
}