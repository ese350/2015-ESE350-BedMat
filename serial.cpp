unsigned int nextTime = 0;    // Next time to read out mat

int rowA = D0;
int rowB = D1;
int rowC = D2;

int colA = D3;
int colB = D4;
int colC = D5;

int inhibit1 = D6;
int inhibit2 = D7;

int data[16][16];

const char commandByte = 0x13; //0b00010011
int resistanceToWrite = 0xFF;

void setup() {
    Serial.begin(9600);
    pinMode(rowA, OUTPUT);
    pinMode(rowB, OUTPUT);
    pinMode(rowC, OUTPUT);
    pinMode(colA, OUTPUT);
    pinMode(colB, OUTPUT);
    pinMode(colC, OUTPUT);

    pinMode(A0, INPUT);
    pinMode(A1, INPUT);

    pinMode(inhibit1, OUTPUT);
    pinMode(inhibit2, OUTPUT);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1);

    // Set initial resistance to 50 ohms
    SPI.transfer(commandByte);
    SPI.transfer(resistanceToWrite);
}

// Zero indexed 0 to 15 inclusive
void switchCol(int col) {
    if (col > 7) {
        // Second half
        digitalWrite(inhibit1, HIGH);
        digitalWrite(inhibit2, LOW);
    } else {
        // Second half
        digitalWrite(inhibit2, HIGH);
        digitalWrite(inhibit1, LOW);
    }

    if (col & 1) {
        // first bit
        digitalWrite(colA, HIGH);
    } else {
        digitalWrite(colA, LOW);
    }

    if ((col >> 1) & 1) {
        // second bit
        digitalWrite(colB, HIGH);
    } else {
        digitalWrite(colB, LOW);
    }

    if ((col >> 2) & 1) {
        // third bit
        digitalWrite(colC, HIGH);
    } else {
        digitalWrite(colC, LOW);
    }
}

// Zero indexed 0 to 7 inclusive (0 will read rows 0 and 8)
void switchRow(int row) {
    if (row & 1) {
        // first bit
        digitalWrite(rowA, HIGH);
    } else {
        digitalWrite(rowA, LOW);
    }

    if ((row >> 1) & 1) {
        // second bit
        digitalWrite(rowB, HIGH);
    } else {
        digitalWrite(rowB, LOW);
    }

    if ((row >> 2) & 1) {
        // third bit
        digitalWrite(rowC, HIGH);
    } else {
        digitalWrite(rowC, LOW);
    }
}


void loop() {
    if (nextTime > millis()) {
        return;
    }

    for (int i = 0; i < 16; i++) {
        // Enable right column
        switchCol(i);

        for (int j = 0; j < 8; j++) {
            switchRow(j);
            
            // wait 1ms
            delay(2);

            // Lower row (0 to 7)
            data[i][j] = analogRead(A0);

            // Higher row (8 to 15)
            data[i][j + 8] = analogRead(A1);
        }
    }

    bool updateResistance = false;

    // Quadrants
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (data[i][j] > 2000) {
                updateResistance = true;
            }

            Serial.print(data[i][j]);
            Serial.print(",");
        }
    }

    Serial.print(resistanceToWrite);

    if (updateResistance) {
        resistanceToWrite--;
        SPI.transfer(commandByte);
        SPI.transfer(resistanceToWrite);
    }

    Serial.println();

    nextTime = millis() + 100;
}