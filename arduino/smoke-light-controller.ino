#define RLED 10
#define GLED 11
#define BLED 9

#define SMOKE 15
#define RASPI 16

enum States {INTRO, CHAOS, FLICKER, STABLE};
States state = INTRO;

int red[1] = {RLED};
int pins[3] = {RLED, GLED, BLED};
unsigned long brokenTime;
int timeFactor;
int piStatus;
unsigned long flickerHighFrequencyMillis = 180000;
unsigned long flickerLowFrequencyMillis = 900000;

void analogWritePins(int pins[3], int intensity) {
    for (int i = 0; i < 3; i++) {
        analogWrite(pins[i], intensity);
    }
}


void allOff() {
    digitalWrite(RLED, 0);
    digitalWrite(GLED, 0);
    digitalWrite(BLED, 0);
}

void toMax(int pin, int initial, int final, int step_us) {
    int intensity = initial;
    for (intensity; intensity < final; intensity++) {
        analogWrite(pin, intensity);
        delayMicroseconds(step_us);
    }
}

void toMax(int pins[3], int initial, int final, int step_us) {
    int intensity = initial;
    for (intensity; intensity < final; intensity++) {
        analogWritePins(pins, intensity);
        delayMicroseconds(step_us);
    }
}

void flicker(int pins[3], int maxIntensity,int cycles, int randRange) {
    for (int cycle = 0; cycle < cycles; cycle++)
    {
        int intensity = random(maxIntensity);
        if (intensity % 2) {
            analogWritePins(pins, intensity);
        }
        else {
            allOff();
        }
        delay(random(randRange));
    }
}

void flickerLoop(int factor) {
    Serial.println("Broken loop running");
    analogWrite(RLED, 1);
    delay(1000);
    toMax(RLED, 1, 255, 20000);
    delay((random(15000) + 5000) * factor);

    flicker(pins, 2, 15, 80);
    analogWritePins(pins, 30);
    delay(random(5000) + 5000);

    flicker(pins, 50, 25, 100);
    analogWritePins(pins, 50);
    delay(random(2000) + 1000);


    flicker(pins, 10, 20, 30);
    toMax(pins, 0, 255, 1000);
    delay(50);
    allOff();
}

void releaseSmoke(int ms) {
    digitalWrite(SMOKE, HIGH);
    delay(ms);
    digitalWrite(SMOKE, LOW);
}

void resetCheck() {
    if (!digitalRead(RASPI)){
        state = INTRO;
        Serial.println("reset");
    }
}

void setup() {
    pinMode(RLED, OUTPUT);
    pinMode(GLED, OUTPUT);
    pinMode(BLED, OUTPUT);
    pinMode(SMOKE, OUTPUT);
    pinMode(RASPI, INPUT);
    digitalWrite(SMOKE, LOW);
    Serial.begin(115200);
}

void loop() {
    switch(state) {
        case INTRO:
            analogWritePins(pins, 255);
            piStatus = digitalRead(RASPI);
            if (piStatus) {
                Serial.println("Signal from pi");
                brokenTime = millis();
                state = CHAOS;
                Serial.println("CHAOS");
            }
            break;
        case CHAOS:
            flicker(pins, 10, 30, 30);
            toMax(pins, 0, 255, 1000);
            delay(50);
            allOff();
            Serial.println("Releasing smoke");
            releaseSmoke(2000);
            analogWrite(RLED, 1);
            releaseSmoke(2000);
            state = FLICKER;
            Serial.println("FLICKER");
            break;
        case FLICKER:
            flickerLoop(timeFactor);
            if ((millis() - brokenTime) < flickerHighFrequencyMillis) {
                timeFactor = 1;
                Serial.println("tf 1");
            }
            else if ((millis() - brokenTime) < flickerLowFrequencyMillis) {
                timeFactor = 3;
                Serial.println("tf 3");
            }
            else {
                state = STABLE;
                Serial.println("STABLE");
            }
            resetCheck();
            break;
        case STABLE:
            analogWritePins(pins, 50);
            resetCheck();
            delay(1000);
    }
}