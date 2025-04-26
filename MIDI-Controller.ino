int CC = 176;

//-----------Rotateable potentionmeter
int dNum[] = {102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117};
int drehWert[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int dWert[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int dAltWert[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int dMpPins[] = {2, 3, 4};
int dBits[] = {0, 0, 0};
int dPins[] = {A0, A1};

//-----------Draggeable potentionmeter
int sNum[] = {85, 86, 87, 88};
int schiebeWert[] = {0, 0, 0, 0};
int sWert[] = {0, 0, 0, 0};
int sAltWert[] = {0, 0, 0, 0};
int sPins[] = {A2, A3, A4, A5};

//-----------Buttons
int bNum[] = {20, 21, 22, 23, 24, 25};
int bPins[] = {5, 6, 7, 8, 9, 10};

//-----------LEDs
int lPins[] = {11, 12};


void setup() {
  for (int i = 0 ; i < 3 ; i++) {
    pinMode(dMpPins[i], OUTPUT); //Multiplexer
  }
  for (int i = 0; i < 2 ; i++) {
    pinMode(dPins[i], INPUT_PULLUP);  //Multiplexer-Input
  }
  for (int i = 0 ; i < 4 ; i++) {
    pinMode(sPins[i], INPUT_PULLUP);  //SchiebePotis-Input
  }
  for (int i = 0 ; i < 7 ; i++) { //Buttons-Input
    pinMode(bPins[i], INPUT);
  }
  for (int i = 0 ; i < 2 ; i++) { //LEDs
    pinMode(lPins[i], OUTPUT);
  }
  Serial.begin(32500);
  digitalWrite(lPins[0], HIGH); //"Power"-LED on 
}

void loop() {
  dPotis();
  sPotis();
  Buttons();
}

void dPotis() {
  for (int i = 0 ; i < 8 ; i++) { // 8 Potis per multiplexer
    for (int j = 0 ; j < 3 ; j++) { // HIGH/LOW configuration for multiplexer
      dBits[j] = bitRead(i, j);
    }
    for (int j = 0 ; j < 3 ; j++) { // Output to multiplexer
      digitalWrite(dMpPins[j], dBits[j]);
    }
    delay(1);
    for (int j = 0 ; j < 2 ; j++) { // 2 multiplexer in total
      drehWert[i + 7 * j] = 0.4 * drehWert[i + 7 * j] + 0.6 * analogRead(dPins[j]); //Read values
      dWert[i + 7 * j] = map(drehWert[i + 7 * j], 0, 1024, 0, 127); //scale to MIDI-values
      delay(1);
      if (abs(dWert[i + 7 * j] - dAltWert[i + 7 * j] > 1)) {  //Check for a new value  
        sendMidi(CC, dNum[i + 7 * j], dWert[i + 7 * j]);  //Send MIDI-data
        dAltWert[i + 7 * j] = dWert[i + 7 * j]; //update actual value
      }
    }
  }
}

void sPotis() {
  for (int i = 0 ; i < 4 ; i++) { // 4 dragable potentiometers
    schiebeWert[i] = 0.4 * schiebeWert[i] + 0.6 * analogRead(sPins[i]); //read values
    sWert[i] = map(schiebeWert[i], 0, 1024, 0, 127);  //scale to MIDI-values
    if (sWert[i] != sAltWert[i]) {  // check for a new value
      sendMidi(CC, sNum[i], sWert[i]);  //Send MIDI-date
    }
    sAltWert[i] = sWert[i]; //update actual value
  }
}

void Buttons() {
  for (int i = 0 ; i < 6 ; i++) { //6 buttons in total
    if (digitalRead(bPins[i]) == HIGH) {  //If button is pressed, send MIDI-Data
      sendMidi(CC, bNum[i], 127);
    }
    else {
      sendMidi(CC, bNum[i], 0);
    }
  }
}

void sendMidi(int Change, int Num, int Wert) { //Send MIDI_data over Serial
  Serial.write(Change);
  Serial.write(Num);
  Serial.write(Wert);

  digitalWrite(lPins[1], HIGH);
  delay(2);
  digitalWrite(lPins[1], LOW);
}
