#define RLED 10
#define GLED 11
#define BLED 9

#define SMOKE 15
#define RASPI 16

#define START 1
#define BROKEN 0

#define NOT_RELEASED 0
#define RELEASED 1

int red[1] = { RLED };
int pins[3] = {RLED, GLED, BLED};
int status = START;
int smoke = NOT_RELEASED;

void analogWritePins(int pins[3], int intensity);
void toMax(int pins[3], int initial, int final, int step_us);
void flicker(int pins[3], int maxIntensity,int cycles, int randRange);
void allOff();
void releaseSmoke(int ms);

void setup()
{
    pinMode(RLED, OUTPUT);
    pinMode(GLED, OUTPUT);
    pinMode(BLED, OUTPUT);
    pinMode(SMOKE, OUTPUT);
    pinMode(RASPI, INPUT);

    digitalWrite(SMOKE, LOW);
    Serial.begin(115200);



}

void loop()
{
    analogWritePins(pins, 255);
    int piStatus;

    while (status == START) {
        piStatus = digitalRead(RASPI);
        if (piStatus) {
            Serial.println("Signal from pi");
            status = BROKEN;
            flicker(pins, 10, 30, 30);
            toMax(pins, 0, 255, 1000);
            delay(50);
            allOff();
        }
    }

    if (smoke == NOT_RELEASED) {
        Serial.println("Releasing smoke");
        releaseSmoke(2000);
        analogWrite(RLED, 1);
        releaseSmoke(2000);
        smoke = RELEASED;
    }

    while (status == BROKEN) {
        Serial.println("Broken loop running");
        analogWrite(RLED, 1);
        delay(1000);
        toMax(red, 1, 255, 10000);
        delay(random(30000) + 10000);

        Serial.println("flicker1");
        flicker(pins, 2, 30, 80);
        analogWritePins(pins, 30);
        delay(random(5000) + 5000);

        Serial.println("flicker2");
        flicker(pins, 50, 40, 100);
        analogWritePins(pins, 50);
        delay(random(2000) + 1000);


        Serial.println("flicker3");
        flicker(pins, 10, 30, 30);
        toMax(pins, 0, 255, 1000);
        delay(50);
        allOff();
    }
}

void analogWritePins(int pins[3], int intensity)
{
    for (int i = 0; i < 3; i++)
    {
        analogWrite(pins[i], intensity);
    }
}

void toMax(int pins[3], int initial, int final, int step_us)
{
    int intensity = initial;
    for (intensity; intensity < final; intensity++) {
        analogWritePins(pins, intensity);
        delayMicroseconds(step_us);
    }
}

void flicker(int pins[3], int maxIntensity,int cycles, int randRange)
{
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

void allOff()
{
    digitalWrite(RLED, 0);
    digitalWrite(GLED, 0);
    digitalWrite(BLED, 0);
}

void releaseSmoke(int ms)
{
    digitalWrite(SMOKE, HIGH);
    delay(ms);
    digitalWrite(SMOKE, LOW);
}