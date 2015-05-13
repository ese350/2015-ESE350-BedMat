#include "HttpClient/HttpClient.h"

unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;

http_header_t headers[] = {
    { "Accept" , "*/*"},
    { NULL, NULL }
};

http_request_t request;
http_response_t response;


int rowA = D0;
int rowB = D1;
int rowC = D2;

int colA = D3;
int colB = D4;
int colC = D5;

int inhibit1 = D6;
int inhibit2 = D7;

int data[16][16];

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

    request.hostname = "INSERT_SERVER_HOSTNAME_HERE";
    request.port = 80;
    request.path = "/pushdata";
}

// Zero indexed 0 to 15 inclusive
void switchCol(int col) {
    Serial.println("SWITCH");
    Serial.println(col);
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

    Serial.println();
    Serial.println("Application>\tStart of Loop. Reading Data...");

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

    // Quadrants
    for (int q = 0; q < 2; q++) {
        for (int r = 0; r < 2; r++) {
            String buffer = "";
            buffer += q*2 + r;
            buffer += ",";

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    buffer += data[8*q + i][8*r + j];
                    buffer += ",";
                }
            }
            
            request.body = buffer;

            // Get request
            http.get(request, response, headers);
        }
    }

    nextTime = millis() + 1000;
}