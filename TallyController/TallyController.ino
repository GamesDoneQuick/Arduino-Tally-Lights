#define TALLY_COUNT 8
uint8_t const PVW_PINS[] = {2, 4, 6, 8, 10, 12, A0, A2};
uint8_t const PGM_PINS[] = {3, 5, 7, 9, 11, 13, A1, A3};
long lastping;

void connectATEMmonitor() {
  digitalWrite(2, HIGH);
  bool connected = false;
  byte incomingByte;
  while (!connected) {
    char buf[256];
    uint8_t buflen = 0;
    while (1) {
      if (Serial.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial.read();

        buf[buflen] = incomingByte;
        buflen++;

        if (incomingByte == '\n') {
          buf[buflen] = 0;
          buflen = 0;
          if (!strcmp(buf, "ATEMmonitor v1.0\n")) {
            digitalWrite(3, HIGH);
            Serial.write("Tally Controller v1.0\n");          

            while (1) {
              if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
        
                buf[buflen] = incomingByte;
                buflen++;
        
                if (incomingByte == '\n') {
                  buf[buflen] = 0;
                  buflen = 0;
                  if (!strcmp(buf, "Handshake OK\n")) {
                    digitalWrite(4, HIGH);
                    connected = true;
                  } else {
                    digitalWrite(2, LOW);
                  }
                  break;
                }
              }
            }
          }
          break;
        }
      }
    }
  }  
  lastping = millis();
}

void setup() {
  for (int i = 0; i < TALLY_COUNT; i++) {
    pinMode(PVW_PINS[i], OUTPUT);
    pinMode(PGM_PINS[i], OUTPUT);
    digitalWrite(PVW_PINS[i], LOW);
    digitalWrite(PGM_PINS[i], LOW);
  }

  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);

  connectATEMmonitor(); 
}


char tallybuffer[TALLY_COUNT + 1];
uint8_t tallybufferlen = 0;

void loop() {
  long now = millis();

  if (Serial.available() > 0) {
    // read the incoming byte:
    byte incomingByte = Serial.read();
    if (incomingByte > '3' || incomingByte < '0') {
      // ERROR
    }

    tallybuffer[tallybufferlen] = incomingByte;
    tallybufferlen++;

    if (tallybufferlen == TALLY_COUNT) {
      for (int i = 0; i < TALLY_COUNT; i++) {
        switch (tallybuffer[i]) {
          case '0':
            digitalWrite(PVW_PINS[i], LOW);
            digitalWrite(PGM_PINS[i], LOW);
            break;
          case '1':
          case '3':
            digitalWrite(PVW_PINS[i], LOW);
            digitalWrite(PGM_PINS[i], HIGH);
            break;
          case '2':
            digitalWrite(PVW_PINS[i], HIGH);
            digitalWrite(PGM_PINS[i], LOW);
            break;
        };
      }
      tallybuffer[tallybufferlen] = 0;
      Serial.println(tallybuffer);
      tallybufferlen = 0;
    }
  }
}
